CPP = c++

CPPFLAGS =  -Wall -Werror -Wextra -fsanitize=address -g

SERVER = server/Server.cpp \
		 server/ServerInfo.cpp \
		 server/ServerConfig.cpp \
		 server/Location.cpp \
		 server/SocketServer.cpp

HTTP = http/Http.cpp

MULTIPLEX = multiplex/Multiplex.cpp \

CLIENT = client/Client.cpp \
		 client/ClientInfo.cpp \
		 client/Response.cpp \
		 client/Request.cpp \
		 client/Cgi.cpp

PARSER = parser/Parser.cpp \
		 parser/ServerContext.cpp \
		 parser/LocationContext.cpp

INCLUDES = -Iincludes 
# Get a list of all subdirectories in the 'includes' directory
#INCLUDE_DIRS := $(wildcard includes/*)

# Build the -I flags for each include directory
#INCLUDES := $(addprefix -I, $(INCLUDE_DIRS))


CPPFILES = webserv.cpp \
		   $(HTTP) \
		   $(SERVER) \
		   $(MULTIPLEX) \
		   $(CLIENT) \
		   $(PARSER)

OBJECTS = $(CPPFILES:.cpp=.o)

NAME = webserv

all : $(NAME)

$(NAME) : $(OBJECTS)
	$(CPP) $(CPPFLAGS) $(INCLUDES) $(OBJECTS) -o $(NAME)

%.o : %.cpp
	$(CPP) $(CPPFLAGS) $(INCLUDES) -c $< -o $@

clean :
	rm -f $(OBJECTS)

fclean : clean
	rm -f $(NAME)

re : fclean all

.PHONY: all clean fclean re
