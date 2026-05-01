# ============================================================
#  Makefile — SIG Bitnópolis (ted)
#  Flags obrigatórias: -std=c99  -fstack-protector-all
# ============================================================

CC       = gcc
CFLAGS   = -std=c99 -fstack-protector-all -Wall -Wextra -g -DUNITY_INCLUDE_DOUBLE
INC      = -I./unity -I./src -I./include

# ---------- fontes do projeto ----------
SRC_DIR  = src
OBJ_DIR  = obj
TST_DIR  = tst

SRC_FILES = hashfile.c quadra.c habitante.c geo.c pm.c qry.c svg.c geo_calc.c main.c
OBJ_FILES = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# ---------- Unity ----------
UNITY    = unity/unity.c

# ========== alvo principal ==========
ted: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

# ========== regra genérica de compilação ==========
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)
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

tst_hashfile: tst/t_hashfile.c $(OBJ_DIR)/hashfile.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_quadra: tst/t_quadra.c $(OBJ_DIR)/quadra.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_habitante: tst/t_habitante.c $(OBJ_DIR)/habitante.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_geo: tst/t_geo.c $(OBJ_DIR)/geo.o $(OBJ_DIR)/quadra.o $(OBJ_DIR)/hashfile.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_pm: tst/t_pm.c $(OBJ_DIR)/pm.o $(OBJ_DIR)/habitante.o $(OBJ_DIR)/hashfile.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_qry: tst/t_qry.c $(OBJ_DIR)/qry.o $(OBJ_DIR)/geo.o $(OBJ_DIR)/pm.o $(OBJ_DIR)/quadra.o $(OBJ_DIR)/habitante.o $(OBJ_DIR)/hashfile.o $(OBJ_DIR)/svg.o $(OBJ_DIR)/geo_calc.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_geo_calc: tst/t_geo_calc.c $(OBJ_DIR)/geo_calc.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

tst_svg: tst/t_svg.c $(OBJ_DIR)/svg.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

# ========== limpeza ==========
clean:
	-del /Q $(OBJ_DIR)\*.o 2>nul
	-del /Q ted.exe tst_hashfile.exe tst_quadra.exe tst_habitante.exe tst_geo.exe tst_pm.exe tst_qry.exe tst_geo_calc.exe tst_svg.exe 2>nul
	-del /Q *.hf *.hfd 2>nul

.PHONY: tstall clean
