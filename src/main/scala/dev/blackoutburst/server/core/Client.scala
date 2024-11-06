package dev.blackoutburst.server.core

import dev.blackoutburst.server.packet.PacketManager

import java.net.Socket

class Client(
    var name: String = "Guest",
    val socket: Socket,
    var renderDistance: Int = 2,
    val entityId: Int
) {
    def read(): Unit =
        val id = socket.getInputStream.readNBytes(1)
        val size = PacketManager.getPacketSize(id(0) & 0xFF).getOrElse(0)
        val body = socket.getInputStream.readNBytes(size)

        if PacketManager.isPacketValid(id, body) then
            PacketManager.decode(id, body)
        else
            socket.close()

    def write(): Unit = {}
}
