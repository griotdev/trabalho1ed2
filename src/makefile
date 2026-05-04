CC       = gcc
CFLAGS   = -std=c99 -fstack-protector-all -Wall -Wextra -g -ggdb -O0 -Werror=implicit-function-declaration -DUNITY_INCLUDE_DOUBLE
INC      = -I../unity -I. -I../include

OBJ_DIR  = ../obj
TST_DIR  = ../tst

SRC_FILES = hashfile.c quadra.c habitante.c geo.c pm.c qry.c svg.c geo_calc.c main.c
OBJ_FILES = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

UNITY    = ../unity/unity.c

# O executável principal deve estar dentro de /src (onde este makefile reside)
ted: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(INC) -o $@ $^ -lm

$(OBJ_DIR)/%.o: %.c
	mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

tstall: tst_hashfile tst_quadra tst_habitante tst_geo tst_pm tst_qry tst_geo_calc tst_svg
	@echo "===== Executando testes ====="
	../tst_hashfile
	../tst_quadra
	../tst_habitante
	../tst_geo
	../tst_pm
	../tst_qry
	../tst_geo_calc
	../tst_svg
	@echo "===== Todos os testes passaram ====="

tst_hashfile: $(TST_DIR)/t_hashfile.c $(OBJ_DIR)/hashfile.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o ../$@ $^ -lm

tst_quadra: $(TST_DIR)/t_quadra.c $(OBJ_DIR)/quadra.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o ../$@ $^ -lm

tst_habitante: $(TST_DIR)/t_habitante.c $(OBJ_DIR)/habitante.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o ../$@ $^ -lm

tst_geo: $(TST_DIR)/t_geo.c $(OBJ_DIR)/geo.o $(OBJ_DIR)/quadra.o $(OBJ_DIR)/hashfile.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o ../$@ $^ -lm

tst_pm: $(TST_DIR)/t_pm.c $(OBJ_DIR)/pm.o $(OBJ_DIR)/habitante.o $(OBJ_DIR)/hashfile.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o ../$@ $^ -lm

tst_qry: $(TST_DIR)/t_qry.c $(OBJ_DIR)/qry.o $(OBJ_DIR)/geo.o $(OBJ_DIR)/pm.o $(OBJ_DIR)/quadra.o $(OBJ_DIR)/habitante.o $(OBJ_DIR)/hashfile.o $(OBJ_DIR)/svg.o $(OBJ_DIR)/geo_calc.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o ../$@ $^ -lm

tst_geo_calc: $(TST_DIR)/t_geo_calc.c $(OBJ_DIR)/geo_calc.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o ../$@ $^ -lm

tst_svg: $(TST_DIR)/t_svg.c $(OBJ_DIR)/svg.o $(UNITY)
	$(CC) $(CFLAGS) $(INC) -o ../$@ $^ -lm

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f ted
	rm -f ../tst_hashfile ../tst_quadra ../tst_habitante ../tst_geo ../tst_pm ../tst_qry ../tst_geo_calc ../tst_svg
	rm -f ../*.hf ../*.hfd

.PHONY: tstall clean
