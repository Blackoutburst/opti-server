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
        if PacketManager.isPacketValid(socket.getInputStream) then
            PacketManager.decode()
        else
            socket.close()

    def write(): Unit = {}
}
