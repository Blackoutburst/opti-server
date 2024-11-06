package dev.blackoutburst.server.packet

class PacketPlayIn(
    val id: Int,
    val size: Int
) {
    def decode(body: Array[Byte]): Unit
}
