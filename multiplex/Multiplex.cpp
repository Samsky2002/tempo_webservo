#include "webserv.hpp"

Multiplex::Multiplex()
{
	// Adding constructor
	quit = false;
	timeout.tv_sec = 5;
	timeout.tv_nsec = 0;
	kq = kqueue();
	if (kq == -1)
		throw(Multiplex::KQUEUE_EXCEPTION());
}

Multiplex::Multiplex( const Multiplex & multiplex )
{
	*this = multiplex;
}

Multiplex & Multiplex::operator=( const Multiplex & multiplex )
{
	if ( this != &multiplex )
	{
		std::cout << "copy assinment multiplex";
		//No need to copy bcs Our Multiplexer should no copiable;
	}
	return ( *this );
}

Multiplex::~Multiplex()
{
	close(kq); // Closing file descriptor of queue that create kqueue();
}

void Multiplex::execute( const Server & server )
{
	setup( server.serverInfo );
	multiplexing( server );
}

void Multiplex::setup( const std::vector< ServerInfo > & serverInfo )
{
	//Changing in setup for kevent
	size_t i = 0;
	std::vector<struct kevent> 	readEvents;
	while ( i < serverInfo.size() )
	{
		memset(&Changeevent, 0, sizeof(Changeevent));
		EV_SET(&Changeevent, serverInfo[i].id, EVFILT_READ, EV_ADD, 0, 0, NULL);
		readEvents.push_back(Changeevent);
		
		i++;
	}
	kevent(kq, readEvents.data(), readEvents.size(), NULL, 0, NULL);
}

bool Multiplex::is_socket( const std::vector< ServerInfo > & serverInfo, int socket )
{
	for ( size_t i = 0; i < serverInfo.size(); i++ )
	{
		if ( socket == serverInfo[i].id )
		{
			return ( true );
		}
	}
	return ( false );
}

void charPointerToVector(const char* charArray, std::vector<char>& charVector, ssize_t len) 
{
	// Assuming charArray is null-terminated
	ssize_t i = 0;
	while ( i < len ) {
		charVector.push_back( charArray[i] );
		i++;
	}
}

void printVec( std::vector<char> & vec )
{
	for ( size_t i = 0; i < vec.size(); i++ )
	{
		std::cout << vec[i];
	}
}


/////////////////////////////////////////////Helper Function///////////////////////////////////////////////
void	Multiplex::accepting(int &fdServer, const Server &server)
{
	int new_client = accept(fdServer, NULL, NULL); 
	if (new_client == -1)
	{
		perror("error in accepting new_connection ");
		return;
	}

	EV_SET(&Changeevent, new_client, EVFILT_READ, EV_ADD, 0, 0, NULL);
	kevent(kq, &Changeevent, 1, NULL, 0, NULL);
	std::cout << "add: " << new_client << std::endl;
	client.addClient( ClientInfo( new_client, server.getServer( fdServer ) ) );
}

void	Multiplex::delete_client(int fdClient)
{
	std::cout << "the client disconnected" << fdClient << std::endl;
	struct kevent Client[2];
	EV_SET(&Client[0], fdClient, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	EV_SET(&Client[1], fdClient, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
	kevent(kq ,Client , 2, NULL, 0, NULL);
	client.removeClient( fdClient );
	close(fdClient);
}

void	Multiplex::receiving(int &i )
{
	char recv_buffer[1024];
	std::vector<char> buffer;

	memset( recv_buffer, '\0', sizeof( recv_buffer ) );
	buffer.clear();
	int rec = recv( i,  recv_buffer, sizeof( recv_buffer ), 0 );
	if ( rec <= 0 )
	{
		if (rec == 0)
		{
			std::cout << "Client Disconnected" << std::endl;
		}
		else
			perror( "receive error: " );
		delete_client(i);
	}
	else
	{
		// recv_buffer[rec] = '\0';
		charPointerToVector( recv_buffer, buffer, rec );
		// printVec(buffer);
		try 
		{
			client.requestSetup( buffer, i );
		}
		catch ( int e ) 
		{
			memset(&Changeevent, 0, sizeof(Changeevent));
			EV_SET(&Changeevent, i, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
			kevent(kq, &Changeevent, 1, NULL, 0, NULL);
		}
	}
}

void	Multiplex::sendig(int &fd)
{
	try 
	{
		client.responseSetup( fd );
	}
	catch ( int e ) 
	{
		std::cout << "test\n";
		std::string send_buffer = client.result( fd );
		int sen = send( fd, send_buffer.c_str(), send_buffer.length(), 0 );
		if ( sen == -1 )
		{
			perror( "send error: " );
		}
		EV_SET(&Changeevent, fd, EVFILT_WRITE, EV_DELETE | EV_CLEAR, 0, 0, NULL);
		kevent(kq, &Changeevent, 1, NULL, 0, NULL);
		EV_SET(&Changeevent, fd, EVFILT_READ, EV_DELETE | EV_CLEAR, 0, 0, NULL);
		kevent(kq, &Changeevent, 1, NULL, 0, NULL);
	}
}

void	Multiplex::multiplexing( const Server &server)
{
	try
	{
		struct kevent signaledEvents[MAX_SOCKET]; // Search A solution
		int nevents;
		while (!quit)
		{
			if (( nevents = kevent(kq, NULL, 0, signaledEvents, MAX_SOCKET, NULL)) == -1 )
			{
				// throw(Multiplex::KEVENT_EXCEPTION());
				continue;
			}
			for (int i = 0; i < nevents; ++i)
			{
				int fd = signaledEvents[i].ident;
				if (signaledEvents[i].filter == EVFILT_READ)
				{
					std::cout << "=============== READ HANDLER ==================" << std::endl;
					if (is_socket(server.serverInfo, fd) )
					{
						std::cout << "==== Accepting NEW CONNECTION ===" << std::endl;
						accepting( fd , server );
					}
					else
						receiving(fd);
				}
				else if (signaledEvents[i].filter == EVFILT_WRITE)
				{
					std::cout << "=============== WRITE HANDLER ==================" << std::endl;
					sendig(fd);
				}
			}
		}
	}
	catch( std::exception &e)
	{
		close(kq);
		std::cout << e.what() << std::endl;
	}
}


const char *Multiplex::KQUEUE_EXCEPTION::what()  const _NOEXCEPT
{
	perror("Kequeue :");
	return ("Faillure on creating kernel queue :") ;
};

const char *Multiplex::KEVENT_EXCEPTION::what() const _NOEXCEPT
{
	perror("Kevent :");
	return ("Faillure in Kevent :") ;
};