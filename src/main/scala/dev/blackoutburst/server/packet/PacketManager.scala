package dev.blackoutburst.server.packet

import dev.blackoutburst.server.packet.client.{C00UpdateEntity, C01UpdateBlock, C02BlockBulkEdit, C03Chat, C04ClientMetadata}

import java.io.InputStream

object PacketManager {

    private val packets = List(
        C00UpdateEntity(0x00, 20),
        C01UpdateBlock(0x01, 13),
        C02BlockBulkEdit(0x02, 0),
        C03Chat(0x03, 4096),
        C04ClientMetadata(0x04, 65),
    )

    def isPacketValid(inputStream: InputStream): Boolean =
        readPacketId(inputStream).flatMap { id =>
            isSizeValid(inputStream, id).map(_ => true)
        }.getOrElse(false)

    private def readPacketId(inputStream: InputStream): Option[Int] =
        Option(inputStream.readNBytes(1))
            .filter(_.nonEmpty)
            .map(_(0) & 0xFF)
            .filter(isIdValid)

    private def getPacketSize(id: Int): Option[Int] = packets.find(_.id == id).map(_.size)

    private def isSizeValid(inputStream: InputStream, id: Int): Option[Boolean] =
        getPacketSize(id).map { size =>
            size == inputStream.readNBytes(size).length
        }.filter(identity)

    private def isIdValid(id: Int): Boolean = packets.exists(_.id == id)

    def decode(): Unit = {}
}
