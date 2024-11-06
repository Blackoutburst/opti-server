package dev.blackoutburst.server.packet.client

import dev.blackoutburst.server.packet.PacketPlayIn

class C04ClientMetadata(id: Int, size: Int) extends PacketPlayIn(id, size) {
    override def decode(body: Array[Byte]): Unit = ???
}
