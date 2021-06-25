NAME	= scop
SRCS	= main.c gl3w.c parser.c shader_handler.c key_hook.c utils.c

OBJ_DIR = ./.Objects
SRC_DIR = ./src
INC_DIR = ./include 
INC_HDR = -I${INC_DIR}

ifeq ($(OS),Windows_NT)
INC_LIB += -lopengl32 -lglfw3 -lglfw3dll -lopengl32 -lgdi32 -Wl,-u,___mingw_vsnprintf -Wl,--defsym,___ms_vsnprintf=___mingw_vsnprintf
else
INC_LIB += -framework OpenGL -lglfw
endif

CFLAGS    := -Wall -Wextra -O3 -I$(INC_DIR) -g
OBJS    := $(patsubst %.c,$(OBJ_DIR)/%.o, $(SRCS))

all: CREATE_OBJDIR $(NAME)

CREATE_OBJDIR:
	@mkdir -p $(OBJ_DIR)

$(NAME): $(OBJS)
	@gcc ${CFLAGS} $^ ${INC_LIB} -o ${NAME} 
	@printf "\033[1m[ SCOP ]\033[1m\t\tcompilation complete \x1b[92m✓\t\033[0m\n"

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c include/scop.h
	@gcc $(CFLAGS) $(INC_HDR) -o $@ -c $<

mk: 
	@mkdir -p .Objects

clean:
	@rm -rf $(OBJ_DIR)
	@echo "\033[1m[ SCOP ]\033[0m\t\tobjects cleared \x1b[92m✓\033[0m"

fclean : clean
	@rm -f $(NAME)
	@echo "\033[1m[ SCOP ]\033[0m\t\tbinary deleted \x1b[92m✓\033[0m"

re: fclean all

ex: all
	@./scop obj/psyduck.obj