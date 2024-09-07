# Project name
NAME 			:= ircserv

# Compiler and flags
CXX				:= g++
CPPFLAGS		:= -Wall -Werror -Wextra -std=c++98
CPPFLAGS		+= -I./include
DEPFLAGS		:= -MMD -MP

# Directories
SRC_DIR			:= ./src
OBJ_DIR			:= ./obj

# Source files
ROOT_SRC		:= \
				Server.cpp \
				Channel.cpp \
				Client.cpp \
				ServerInit.cpp \
				main.cpp
ROOT_DIR		:= $(SRC_DIR)
ROOT_OBJ		:= $(addprefix $(OBJ_DIR)/, $(ROOT_SRC:.cpp=.o))

# Combine all source files and object files
SRC				:= \
				$(ROOT_SRC)
OBJ				:= \
				$(ROOT_OBJ)

# Rule to compile source files into object files
$(OBJ_DIR)/%.o:	$(SRC_DIR)/%.cpp
				@mkdir -p $(@D)
				$(CXX) $(CPPFLAGS) $(DEPFLAGS) -c $< -o $@

# Default target: build the executable
all:
				make $(NAME)

# Rule to build the executable
$(NAME):		$(OBJ)
				$(CXX) $(CPPFLAGS) $(DEPFLAGS) -o $(NAME) $(OBJ)

# Clean object files and dependencies
clean:
				rm -rf $(OBJ_DIR)

# Remove object files, dependencies, and the executable
fclean:			clean
				rm -f $(NAME)

# Rebuild everything
re:				fclean
				make all

# Release target with optimization and stripping
release:		CPPFLAGS += -O2
release:		$(NAME)
				strip $(NAME)

# Debug and Optimization Mode Configuration
DEBUG			:= 0

ifeq ($(DEBUG), 1)
	CPPFLAGS 	+= -g3
	CXX			:= g++
endif

# Phony targets
.PHONY:			all clean debug fclean re release

# Include dependency files
-include $(OBJ:.o=.d)
