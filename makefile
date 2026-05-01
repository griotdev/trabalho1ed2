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
           src/geo.c       \
           src/pm.c        \
           src/qry.c       \
           src/svg.c       \
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
tstall: tst_hashfile tst_quadra tst_habitante tst_geo tst_pm tst_qry tst_geo_calc tst_svg
	@echo "===== Executando testes ====="
	./tst_hashfile
	./tst_quadra
	./tst_habitante
	./tst_geo
	./tst_pm
	./tst_qry
	./tst_geo_calc
	./tst_svg
	@echo "===== Todos os testes passaram ====="

tst_hashfile: tst/t_hashfile.c src/hashfile.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_quadra: tst/t_quadra.c src/quadra.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_habitante: tst/t_habitante.c src/habitante.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_geo: tst/t_geo.c src/geo.c src/quadra.c src/hashfile.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_pm: tst/t_pm.c src/pm.c src/habitante.c src/hashfile.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_qry: tst/t_qry.c src/qry.c src/geo.c src/pm.c src/quadra.c src/habitante.c src/hashfile.c src/svg.c src/geo_calc.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_geo_calc: tst/t_geo_calc.c src/geo_calc.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_svg: tst/t_svg.c src/svg.c $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

# ========== limpeza ==========
clean:
	-del /Q src\*.o 2>nul
	-del /Q ted.exe tst_hashfile.exe tst_quadra.exe tst_habitante.exe tst_geo.exe tst_pm.exe tst_qry.exe tst_geo_calc.exe tst_svg.exe 2>nul
	-del /Q *.hf *.hfd 2>nul

.PHONY: tstall clean
