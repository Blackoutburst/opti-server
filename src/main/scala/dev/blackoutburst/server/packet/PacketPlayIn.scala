package dev.blackoutburst.server.packet

abstract class PacketPlayIn(
    val id: Int,
    val size: Int
) {
    def decode(body: Array[Byte]): Unit
}
