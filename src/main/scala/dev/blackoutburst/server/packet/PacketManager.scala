package dev.blackoutburst.server.packet

import dev.blackoutburst.server.packet.client.{C00UpdateEntity, C01UpdateBlock, C02BlockBulkEdit, C03Chat, C04ClientMetadata}

object PacketManager {

    private val packets = List(
        C00UpdateEntity(0x00, 20),
        C01UpdateBlock(0x01, 13),
        C02BlockBulkEdit(0x02, 0),
        C03Chat(0x03, 4096),
        C04ClientMetadata(0x04, 65),
    )

    def isPacketValid(id: Array[Byte], body: Array[Byte]): Boolean =
        checkPacketId(id).flatMap { id =>
            isSizeValid(id, body).map(_ => true)
        }.getOrElse(false)

    private def checkPacketId(id: Array[Byte]): Option[Int] =
        Option(id)
            .filter(_.nonEmpty)
            .map(_(0) & 0xFF)
            .filter(isIdValid)

    def getPacketSize(id: Int): Option[Int] = packets.find(_.id == id).map(_.size)

    private def isSizeValid(id: Int, body: Array[Byte]): Option[Boolean] =
        getPacketSize(id).map { size =>
            size == body.length
        }.filter(identity)

    private def isIdValid(id: Int): Boolean = packets.exists(_.id == id)

    def decode(id: Array[Byte], body: Array[Byte]): Unit =
        packets.find(_.id == (id(0) & 0xFF)) match
            case Some(packet) => packet.decode(body)
            case None => println(f"Tf happened ? Wrong packet id [${id(0) & 0xFF}]")

}
