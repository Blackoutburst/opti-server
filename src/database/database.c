#include <string.h>
#include <stdlib.h>
#include "database/database.h"
#include "sqlite/sqlite3.h"
#include "utils/ioUtils.h"

static sqlite3* db = NULL;
static I32 rc = 0;
static I8* errMsg = NULL;

U8* dbGetChunkBlocks(I32 x, I32 y, I32 z) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT blocks FROM chunks WHERE x = ? AND y = ? AND z = ?;";
     if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        println("SELECT blocks FROM chunks WHERE x = ? AND y = ? AND z = ?; fucked up");
        return NULL;
     }

    sqlite3_bind_int(stmt, 1, x);
    sqlite3_bind_int(stmt, 2, y);
    sqlite3_bind_int(stmt, 3, z);

    const U8* data = NULL;
    U8* blocks = NULL;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        data = sqlite3_column_text(stmt, 0);
    }

    if (data != NULL) {
        blocks = malloc(CHUNK_BLOCK_COUNT);
        memcpy(blocks, data, CHUNK_BLOCK_COUNT);
    }

    sqlite3_finalize(stmt);

    return blocks;
}

void dbAddChunk(CHUNK* chunk) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO chunks (x, y, z, blocks) VALUES (?, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        println("INSERT INTO chunks (x, y, z, blocks) VALUES (?, ?, ?, ?); fucked up");
        return;
    }

    sqlite3_bind_int(stmt, 1, chunk->position.x);
    sqlite3_bind_int(stmt, 2, chunk->position.y);
    sqlite3_bind_int(stmt, 3, chunk->position.z);

    I8 blocksStr[CHUNK_BLOCK_COUNT] = {0};
    if (chunk->monotype) {
        for (int i = 0; i < CHUNK_BLOCK_COUNT; i++) blocksStr[i] = chunk->blocks[0];
    } else {
        memcpy(blocksStr, chunk->blocks, CHUNK_BLOCK_COUNT);
    }
    
    sqlite3_bind_text(stmt, 4, blocksStr, CHUNK_BLOCK_COUNT, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        printf("Failed to insert chunk: %i, %i, %i\n", chunk->position.x, chunk->position.y, chunk->position.z);
    }

    sqlite3_finalize(stmt);
}

void _dbCreateWorldTable(void) {
    const char *sql = "CREATE TABLE IF NOT EXISTS world ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "seed INTEGER);";
    rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);

    if (rc != SQLITE_OK) {
        println("Couldn't create [world] table");
        exit(1);
    }
}

void _dbCreateChunkTable(void) {
    const char *sql = "CREATE TABLE IF NOT EXISTS chunks ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "x INTEGER, "
                  "y INTEGER, "
                  "z INTEGER, "
                  "blocks TEXT);";
    rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);

    if (rc != SQLITE_OK) {
        println("Couldn't create [chunks] table");
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
    rc = sqlite3_open("sqlite.db", &db);
    if (rc != SQLITE_OK) {
        println("Couldn't open [sqlite.db]");
        exit(1);
    }

    println("Connected to local database [sqlite.db]");
    
    dbCreateTables();
}
