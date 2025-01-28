## Information
- This protocol uses **BIG ENDIAN**
- Server doesn't send empty chunk
- TCP uses port 15000

## Current Protocol

### Client bound

Identification: `0x00`
| id   | entityId |
|------|----------|
| byte | int      |

Add Entity: `0x01`
| id   | entityId | x     | y     | z     | yaw   | pitch | name     |
|------|----------|-------|-------|-------|-------|-------|----------|
| byte | int      | float | float | float | float | float | byte[64] |

Remove Entity: `0x02`
| id   | entityId |
|------|----------|
| byte | int      |


Update Entity Position: `0x03`
| id   | entityId | x     | y     | z     | yaw   | pitch |
|------|----------|-------|-------|-------|-------|-------|
| byte | int      | float | float | float | float | float |

Send Chunk: `0x04`
| id   | x   | y   | z   | BlockType  |
|------|-----|-----|-----|------------|
| byte | int | int | int | byte[4096] |

Send Mono Type Chunk: `0x05`
| id   | x   | y   | z   | BlockType |
|------|-----|-----|-----|-----------|
| byte | int | int | int | byte      |

Chat: `0x06`
| id   | message    |
|------|------------|
| byte | byte[4096] |

Update Entity Metadata: `0x07`
| id   | entityId | name     |
|------|----------|----------|
| byte | int      | byte[64] |

### Server bound
Update Entity: `0x00`
| id   | x     | y     | z     | yaw   | pitch |
|------|-------|-------|-------|-------|-------|
| byte | float | float | float | float | float |

Update Block: `0x01`
| id   | BlockType | x   | y   | z   |
|------|-----------|-----|-----|-----|
| byte | byte      | int | int | int |

Block Bulk Edit: `0x02`
| id   | blockCount | BlockType | x   | y   | z   | BlockType | x   | y   | z   | ... |
|------|------------|-----------|-----|-----|-----|-----------|-----|-----|-----|-----|
| byte | Int        | byte      | int | int | int | byte      | int | int | int | ... |

Chat: `0x03`
| id   | message    |
|------|------------|
| byte | byte[4096] |

Client metadata: `0x04`
| id   | renderDistance | name       |
|------|----------------|------------|
| byte | byte           | byte[64]   |


### BlockType
| id | Name         |
|----|--------------|
| 0  | Air          |
| 1  | Grass        |
| 2  | Dirt         |
| 3  | Stone        |
| 4  | Oak Log      |
| 5  | Oak Leaves   |
| 6  | Glass        |
| 7  | Water        |
| 8  | Sand         |
| 9  | Snow         |
| 10 | Oak Planks   |
| 11 | Stone Bricks |
| 12 | Netherrack   |
| 13 | Gold Block   |
| 14 | Packed Ice   |
| 15 | Lava         |
| 16 | Barrel       |
| 17 | Bookshelf    |

