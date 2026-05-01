# ============================================================
#  Makefile — SIG Bitnópolis (ted)
#  Flags obrigatórias: -std=c99  -fstack-protector-all
# ============================================================

CC       = gcc
CFLAGS   = -std=c99 -fstack-protector-all -Wall -Wextra -g -DUNITY_INCLUDE_DOUBLE
INC      = -I./unity -I./src -I./include

# ---------- fontes do projeto ----------
SRC      = src/hashfile.c  \
           src/quadra.c    \
           src/habitante.c \
           src/morador.c   \
           src/parser_geo.c \
           src/parser_pm.c  \
           src/parser_qry.c \
           src/svg.c        \
           src/geo_calc.c

OBJ      = $(SRC:.c=.o)

# ---------- Unity ----------
UNITY    = unity/unity.c

# ========== alvo principal ==========
ted: src/main.o $(OBJ)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

# ========== regra genérica de compilação ==========
src/%.o: src/%.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# ========== testes unitários ==========
tstall: tst_hashfile tst_quadra tst_habitante tst_morador
	@echo "===== Executando testes ====="
	./tst_hashfile
	./tst_quadra
	./tst_habitante
	./tst_morador
	@echo "===== Todos os testes passaram ====="

tst_hashfile: tst/t_hashfile.c src/hashfile.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_quadra: tst/t_quadra.c src/quadra.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_habitante: tst/t_habitante.c src/habitante.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_morador: tst/t_morador.c src/morador.c src/habitante.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

# ========== limpeza ==========
clean:
	-del /Q src\*.o 2>nul
	-del /Q ted.exe tst_hashfile.exe tst_quadra.exe tst_habitante.exe tst_morador.exe 2>nul
	-del /Q *.hf *.hfd 2>nul

.PHONY: tstall clean
