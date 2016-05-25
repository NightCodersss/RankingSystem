import simpleubjson
import SocketServer
import socket
import time
import os
import nltk
import stem_provider

listen_point = ("localhost", int(os.environ["STEM_PORT"]))
south_server_point = ("localhost", int(os.environ["SOUTH_PORT"]))


class MyTCPHandler(SocketServer.BaseRequestHandler):
	def handle(self):
        # self.request is the TCP socket connected to the client
		query = ""
		while True:
			buf = self.request.recv(1024)
			query += buf
			if buf.find('\n') != -1:
				break
		#print "query: ", query.decode('utf-8')
		# do stem
		query = ' '.join(map(lambda s: stem_provider.stem(s) if not s in ['|', ')', '('] else s, nltk.word_tokenize(query.decode('utf-8')))) + '\n'
	#	print "stemmed query: ", query
		
		sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		print "connecting to south"
		sock.connect(south_server_point)
		print "connected"
		print "sending"
		sock.sendall(query.encode('utf-8'))
		print "sent; waiting"
		time.sleep(1) # 1 sec waiting for (may be notfinished) ubjson answer
		print "receiving data"
		answer = ""
		while True:
			print "waiting for data (north)"
			buf = sock.recv(1024)
			answer += buf 
			if len(buf) == 0:
				break
			print "answer_data: ", answer
		print "Got answer"
		self.request.sendall(answer)

if __name__ == "__main__":
    server = SocketServer.ThreadingTCPServer(listen_point, MyTCPHandler)
    server.serve_forever()

