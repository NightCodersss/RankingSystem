import simpleubjson
import SocketServer
import socket
import time
import os
import nltk
import nltk.probability
import nltk.stem
import nltk.corpus
import thread

listen_point = ("localhost", int(os.environ["STEM_PORT"]))
south_server_point = ("localhost", int(os.environ["SOUTH_PORT"]))

stemmer = nltk.stem.SnowballStemmer("russian")

class MyTCPHandler(SocketServer.BaseRequestHandler):
	def handle(self):
		thread.start_new_thread(lambda: self.routine(), ())

	def routine(self):
        # self.request is the TCP socket connected to the client
		query = ""
		while True:
			buf = self.request.recv(1024)
			query += buf
			if buf.find('\n') != -1:
				break
		print "query: ", query
		# do stem
		query = ' '.join(map(stemmer.stem, nltk.word_tokenize(query)))
		print "stemmed query: ", query
		
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		sock.connect(south_server_point)
		sock.sendall(query)
		time.sleep(1) # 1 sec waiting for (may be notfinished) ubjson answer
		answer = ""
		while True:
#			print "waiting for data (north)"
			buf = sock.recv(1024)
			answer += buf 
			if len(buf) == 0:
				break
#			print "answer_data: ", answer
		print "Got answer"
		self.request.sendall(answer)

if __name__ == "__main__":
    server = SocketServer.TCPServer(listen_point, MyTCPHandler)
    server.serve_forever()

