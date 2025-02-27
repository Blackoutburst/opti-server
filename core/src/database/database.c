#include <string.h>
#include <stdlib.h>
#include "database/database.h"
#include "sqlite/sqlite3.h"
#include "utils/logger.h"
#include "utils/args.h"
#include "world/world.h"

static sqlite3* db = NULL;
static I32 rc = 0;
static I8* errMsg = NULL;

typedef struct chunkhash CHUNKHASH;
struct chunkhash {
    int hash[3];
};

U64 encode3int21(I32 x, I32 y, I32 z) {
    #define OFFSET (0x1FFFFF / 2)

    return (U64)(
        ((U64)((x + OFFSET) & 0x1FFFFF))       |
        ((U64)((y + OFFSET) & 0x1FFFFF)) << 21 |
        ((U64)((z + OFFSET) & 0x1FFFFF)) << 42
    );
}

void dbGetChunksInRegion(TCP_CLIENT* client, I32 minX, I32 maxX, I32 minY, I32 maxY, I32 minZ, I32 maxZ) {
    sqlite3_stmt* stmt;

    // const I8* sql = "SELECT x, y, z, blocks FROM chunks WHERE x = ? AND y = ? AND z = ?;";
    const I8* sql = "SELECT hash, blocks FROM chunks WHERE hash = ?;";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        logE("Failed to prepare statement for dbGetChunksInRegion error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

    int count = 0;

    for (I32 x = minX; x < maxX; x += CHUNK_SIZE) {
    for (I32 y = minY; y < maxY; y += CHUNK_SIZE) {
    for (I32 z = minZ; z < maxZ; z += CHUNK_SIZE) {
        if (worldGetChunk(client, x, y, z) != 0) continue;

        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt); // can be commented

        U64 hash = encode3int21(x / CHUNK_SIZE, y / CHUNK_SIZE, z / CHUNK_SIZE);
        sqlite3_bind_int64(stmt, 1, hash);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            count += 1;

            // I32 x = sqlite3_column_int(stmt, 1);
            // I32 y = sqlite3_column_int(stmt, 2);
            // I32 z = sqlite3_column_int(stmt, 3);
            if (worldGetChunk(client, x, y, z)) continue;

            const U8* data = sqlite3_column_blob(stmt, 1);


            if (data) {
                U8* blocks = malloc(CHUNK_BLOCK_COUNT);
                memcpy(blocks, data, CHUNK_BLOCK_COUNT);

                insert(&client->dbChunks, ((VECTORI){x, y, z}), blocks);
            }
        }
    }}}

    // logD("chunks reads: %d", count);

    sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

    sqlite3_finalize(stmt);
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
    const I8* sql = "INSERT OR REPLACE INTO chunks (hash, x, y, z, blocks) VALUES (?, ?, ?, ?, ?);";
    I32 rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        logE("Failed to prepare statement for dbAddChunks error: %s", sqlite3_errmsg(db));
        return;
    }

    sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

    for (U32 i = 0; i < count; i++) {
        U64 hash = encode3int21(chunks[i]->position.x / CHUNK_SIZE, chunks[i]->position.y / CHUNK_SIZE, chunks[i]->position.z / CHUNK_SIZE);
        sqlite3_bind_int64(stmt, 1, hash);

        sqlite3_bind_int(stmt, 2, chunks[i]->position.x);
        sqlite3_bind_int(stmt, 3, chunks[i]->position.y);
        sqlite3_bind_int(stmt, 4, chunks[i]->position.z);

        I8 blocksStr[CHUNK_BLOCK_COUNT];
        if (chunks[i]->monotype) {
            for (I32 j = 0; j < CHUNK_BLOCK_COUNT; j++) {
                blocksStr[j] = chunks[i]->blocks[0];
            }
        } else {
            memcpy(blocksStr, chunks[i]->blocks, CHUNK_BLOCK_COUNT);
        }

        sqlite3_bind_blob(stmt, 5, blocksStr, CHUNK_BLOCK_COUNT, SQLITE_STATIC);

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
    const I8* sql = "INSERT OR REPLACE INTO chunks (hash, x, y, z, blocks) VALUES (?, ?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        logE("Failed to prepare statement for dbAddChunk error: %s", sqlite3_errmsg(db));
        return;
    }

    U64 hash = encode3int21(chunk->position.x / CHUNK_SIZE, chunk->position.y / CHUNK_SIZE, chunk->position.z / CHUNK_SIZE);

    sqlite3_bind_int64(stmt, 1, hash);

    sqlite3_bind_int(stmt, 2, chunk->position.x);
    sqlite3_bind_int(stmt, 3, chunk->position.y);
    sqlite3_bind_int(stmt, 4, chunk->position.z);

    I8 blocksStr[CHUNK_BLOCK_COUNT] = {0};
    if (chunk->monotype) {
        for (I32 i = 0; i < CHUNK_BLOCK_COUNT; i++) blocksStr[i] = chunk->blocks[0];
    } else {
        memcpy(blocksStr, chunk->blocks, CHUNK_BLOCK_COUNT);
    }

    sqlite3_bind_blob(stmt, 5, blocksStr, CHUNK_BLOCK_COUNT, SQLITE_STATIC);

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
                  "hash INTEGER PRIMARY KEY, "
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
