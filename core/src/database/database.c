#include <string.h>
#include <stdlib.h>
#include "database/database.h"
#include "sqlite/sqlite3.h"
#include "utils/logger.h"
#include "utils/args.h"
#include "world/world.h"
#include "utils/perfTimer.h"

static sqlite3* db = NULL;
static I32 rc = 0;
static I8* errMsg = NULL;

void dbGetChunksInRegion(TCP_CLIENT* client, const vec(VECTORI)* chunksToGet) {
    const char *createTempTableSQL = "CREATE TEMP TABLE temp_points (dx INTEGER, dy INTEGER, dz INTEGER);";
    if (sqlite3_exec(db, createTempTableSQL, 0, 0, NULL) != SQLITE_OK) {
        logE("Failed to sqlite3_exec error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlite3_stmt *insertStmt;
    const char *insertSQL = "INSERT INTO temp_points VALUES (?, ?, ?);";
    if (sqlite3_prepare_v2(db, insertSQL, -1, &insertStmt, NULL) != SQLITE_OK) {
        logE("sqlite3_prepare_v2: insertSQL");
    }

    const I32 chunksToGetSize = size(chunksToGet);
    logD("chunksToGetSize: %d", chunksToGetSize);
    for (I32 i = 0 ; i < chunksToGetSize ; i++) {
        VECTORI v = *get(chunksToGet, i);

        sqlite3_bind_int(insertStmt, (i*3)+0, v.x);
        sqlite3_bind_int(insertStmt, (i*3)+1, v.y);
        sqlite3_bind_int(insertStmt, (i*3)+2, v.z);

        if (sqlite3_step(insertStmt) != SQLITE_DONE) {
            // handle error
            logE("sqlite3_step");
        }
        sqlite3_reset(insertStmt);
    }

    sqlite3_finalize(insertStmt);

    perfTimerBegin("dbGetChunksInRegion");

    sqlite3_stmt *selectStmt;
    const char *selectSQL = "SELECT x, y, z, blocks FROM chunks LEFT JOIN temp_points ON x = dx AND y = dy AND z = dz;";

    if (sqlite3_prepare_v2(db, selectSQL, -1, &selectStmt, NULL) != SQLITE_OK) {
        logE("Failed to prepare statement for dbGetChunksInRegion error: %s", sqlite3_errmsg(db));
    }

    // if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    //     logE("Failed to prepare statement for dbGetChunksInRegion error: %s", sqlite3_errmsg(db));
    //     return;
    // }

    while (sqlite3_step(selectStmt) == SQLITE_ROW) {
        I32 x = sqlite3_column_int(selectStmt, 0);
        I32 y = sqlite3_column_int(selectStmt, 1);
        I32 z = sqlite3_column_int(selectStmt, 2);
        if (worldGetChunk(client, x, y, z)) {
            continue;
        }

        const U8* data = sqlite3_column_blob(selectStmt, 3);

        if (data) {
            U8* blocks = malloc(CHUNK_BLOCK_COUNT);
            memcpy(blocks, data, CHUNK_BLOCK_COUNT);

            insert(&client->dbChunks, chunkHash(x, y, z), blocks);
        }
    }

    sqlite3_finalize(selectStmt);
    perfTimerEnd();

    sqlite3_exec(db, "DROP TABLE IF EXISTS temp_points;", NULL, NULL, NULL);
}

U8* dbGetChunkBlocks(I32 x, I32 y, I32 z) {
    sqlite3_stmt* stmt;
    const I8* sql = "SELECT blocks FROM chunks WHERE x = ? AND y = ? AND z = ?;";
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        logE("Failed to prepare statement for dbGetChunkBlocks error: %s", sqlite3_errmsg(db));
        return NULL;
     }

    sqlite3_bind_int(stmt, 1, x);
    sqlite3_bind_int(stmt, 2, y);
    sqlite3_bind_int(stmt, 3, z);

    const U8* data = NULL;
    U8* blocks = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        data = sqlite3_column_blob(stmt, 0);
    }

    if (data != NULL) {
        blocks = malloc(CHUNK_BLOCK_COUNT);
        memcpy(blocks, data, CHUNK_BLOCK_COUNT);
    }

    sqlite3_finalize(stmt);

    return blocks;
}

void dbAddChunks(CHUNK** chunks, U32 count) {
    sqlite3_stmt* stmt = NULL;
    const I8* sql = "INSERT OR REPLACE INTO chunks (x, y, z, blocks) VALUES (?, ?, ?, ?);";
    I32 rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        logE("Failed to prepare statement for dbAddChunks error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

    for (U32 i = 0; i < count; i++) {
        sqlite3_bind_int(stmt, 1, chunks[i]->position.x);
        sqlite3_bind_int(stmt, 2, chunks[i]->position.y);
        sqlite3_bind_int(stmt, 3, chunks[i]->position.z);

        I8 blocksStr[CHUNK_BLOCK_COUNT];
        if (chunks[i]->monotype) {
            for (I32 j = 0; j < CHUNK_BLOCK_COUNT; j++) {
                blocksStr[j] = chunks[i]->blocks[0];
            }
        } else {
            memcpy(blocksStr, chunks[i]->blocks, CHUNK_BLOCK_COUNT);
        }

        sqlite3_bind_blob(stmt, 4, blocksStr, CHUNK_BLOCK_COUNT, SQLITE_STATIC);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE) {
            logW("Failed to insert chunk at (%i, %i, %i): %s", chunks[i]->position.x, chunks[i]->position.y, chunks[i]->position.z, sqlite3_errmsg(db));
        }

        sqlite3_reset(stmt);
        chunkClean(chunks[i]);
    }

    sqlite3_exec(db, "COMMIT;", NULL, NULL, NULL);
    sqlite3_finalize(stmt);
}

void dbAddChunk(CHUNK* chunk) {
    sqlite3_stmt* stmt;
    const I8* sql = "INSERT OR REPLACE INTO chunks (x, y, z, blocks) VALUES (?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        logE("Failed to prepare statement for dbAddChunk error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlite3_bind_int(stmt, 1, chunk->position.x);
    sqlite3_bind_int(stmt, 2, chunk->position.y);
    sqlite3_bind_int(stmt, 3, chunk->position.z);

    I8 blocksStr[CHUNK_BLOCK_COUNT] = {0};
    if (chunk->monotype) {
        for (I32 i = 0; i < CHUNK_BLOCK_COUNT; i++) blocksStr[i] = chunk->blocks[0];
    } else {
        memcpy(blocksStr, chunk->blocks, CHUNK_BLOCK_COUNT);
    }

    sqlite3_bind_blob(stmt, 4, blocksStr, CHUNK_BLOCK_COUNT, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        logW("Failed to insert chunk at (%i, %i, %i): %s", chunk->position.x, chunk->position.y, chunk->position.z, sqlite3_errmsg(db));
    }

    sqlite3_finalize(stmt);
}

void _dbCreateWorldTable(void) {
    const I8* sql = "CREATE TABLE IF NOT EXISTS world ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "seed INTEGER);";
    rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);

    if (rc != SQLITE_OK) {
        logE("Couldn't create [world] table");
        exit(1);
    }
}

void _dbCreateChunkTable(void) {
    const I8* sql = "CREATE TABLE IF NOT EXISTS chunks ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "x INTEGER, "
                  "y INTEGER, "
                  "z INTEGER, "
                  "blocks BLOB, "
                  "UNIQUE(x, y, z) ON CONFLICT REPLACE"
                  ");";
    rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);

    if (rc != SQLITE_OK) {
        logE("Couldn't create [chunks] table");
        exit(1);
    }
}

void dbCreateTables(void) {
    _dbCreateWorldTable();
    _dbCreateChunkTable();
}

void dbClean(void) {
    if (db == NULL) return;
    sqlite3_close(db);

    db = NULL;
}

void dbInit(void) {
    if (db != NULL) return;
    rc = sqlite3_open(argsGetDbType() == DB_FILE ? "sqlite.db" : ":memory:", &db);
    if (rc != SQLITE_OK) {
        logE("Couldn't open database");
        exit(1);
    }

    sqlite3_exec(db, "PRAGMA journal_mode = MEMORY;", NULL, NULL, NULL);
    sqlite3_exec(db, "PRAGMA synchronous = OFF;", NULL, NULL, NULL);
    sqlite3_exec(db, "PRAGMA foreign_keys = OFF;", NULL, NULL, NULL);
    sqlite3_exec(db, "PRAGMA ignore_check_constraints = 1;", NULL, NULL, NULL);
    sqlite3_exec(db, "PRAGMA locking_mode = EXCLUSIVE;", NULL, NULL, NULL);
    sqlite3_exec(db, "PRAGMA temp_store = MEMORY;", NULL, NULL, NULL);

    logI("Connected to local database");

    dbCreateTables();
}
