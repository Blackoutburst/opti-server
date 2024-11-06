package dev.blackoutburst.server.core

import java.net.ServerSocket
import scala.concurrent.{ExecutionContext, Future}

object TCPCore {
    private var clients: List[Client] = Nil

    private def readClient(client: Client): Future[Unit] = Future {
        while (!client.socket.isClosed) {
            client.read()
        }
    }(ExecutionContext.global)

    def createServer(): Future[Unit] = Future {
        val server = ServerSocket(15000)

        while (true) {
            val socket = server.accept()
            val client = Client(socket = socket, entityId = -1)

            readClient(client)

            synchronized {
                clients = client :: clients
            }
        }
    }(ExecutionContext.global)
}
