# Modify the Makefile as required

CXX=gcc
CFLAGS=-g -Wall

INCDIR=lib
OBJDIR=obj

build: $(OBJDIR)/uthread

$(OBJDIR)/ustructs.o: $(INCDIR)/ustructs.c
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -c -o $@

$(OBJDIR)/uthread.o: $(INCDIR)/uthread.c
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -c -o $@

$(OBJDIR)/uthread: $(OBJDIR)/ustructs.o $(OBJDIR)/uthread.o
	ar rcs $@ $^ 

# Run `make clean` everytime

#########################################

# Intermediate testcases
# T1
$(OBJDIR)/inter_t1.o: testcases/inter/t1.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

inter_t1: $(OBJDIR)/inter_t1.o 
	./$(OBJDIR)/inter_t1.o RR 1000

# T2
$(OBJDIR)/inter_t2.o: testcases/inter/t2.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

inter_t2: $(OBJDIR)/inter_t2.o
	./$(OBJDIR)/inter_t2.o RR 1000

#########################################

# Final testcases
# RR T1
$(OBJDIR)/rr_t1.o: testcases/final/rr/t1.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

rr_t1: $(OBJDIR)/rr_t1.o 
	./$(OBJDIR)/rr_t1.o RR 1000

# RR T2
$(OBJDIR)/rr_t2.o: testcases/final/rr/t2.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

rr_t2: $(OBJDIR)/rr_t2.o
	./$(OBJDIR)/rr_t2.o RR 1000

# RR T3
$(OBJDIR)/rr_t3.o: testcases/final/rr/t3.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

rr_t3: $(OBJDIR)/rr_t3.o
	./$(OBJDIR)/rr_t3.o RR 1000

# RR T4
$(OBJDIR)/rr_t4.o: testcases/final/rr/t4.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

rr_t4: $(OBJDIR)/rr_t4.o
	./$(OBJDIR)/rr_t4.o RR 1000

#########################################

# SP T1
$(OBJDIR)/sp_t1.o: testcases/final/sp/t1.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

sp_t1: $(OBJDIR)/sp_t1.o 
	./$(OBJDIR)/sp_t1.o SP 1000

# SP T2
$(OBJDIR)/sp_t2.o: testcases/final/sp/t2.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

sp_t2: $(OBJDIR)/sp_t2.o
	./$(OBJDIR)/sp_t2.o SP 1000

# SP T3
$(OBJDIR)/sp_t3.o: testcases/final/sp/t3.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

sp_t3: $(OBJDIR)/sp_t3.o
	./$(OBJDIR)/sp_t3.o SP 1000

# SP T4
$(OBJDIR)/sp_t4.o: testcases/final/sp/t4.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

sp_t4: $(OBJDIR)/sp_t4.o
	./$(OBJDIR)/sp_t4.o SP 1000

#########################################

# DP T1
$(OBJDIR)/dp_t1.o: testcases/final/dp/t1.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

dp_t1: $(OBJDIR)/dp_t1.o 
	./$(OBJDIR)/dp_t1.o DP 1000 2

# DP T2
$(OBJDIR)/dp_t2.o: testcases/final/dp/t2.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

dp_t2: $(OBJDIR)/dp_t2.o
	./$(OBJDIR)/dp_t2.o DP 1000 2

# DP T3
$(OBJDIR)/dp_t3.o: testcases/final/dp/t3.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

dp_t3: $(OBJDIR)/dp_t3.o
	./$(OBJDIR)/dp_t3.o DP 1000 2

# DP T4
$(OBJDIR)/dp_t4.o: testcases/final/dp/t4.c $(OBJDIR)/uthread
	$(CXX) $(CFLAGS) -I$(INCDIR) $^ -o $@

dp_t4: $(OBJDIR)/dp_t4.o
	./$(OBJDIR)/dp_t4.o DP 1000 2


.PHONY: clean

clean:
	rm -f *.o obj/*