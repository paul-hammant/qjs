.PHONY: all clean ping_pong skynet

# Use standard Erlang compilation for fair baseline comparison
ERLC = erlc
ERLC_FLAGS =
ERL = erl
ERL_FLAGS = -noshell

all: ping_pong skynet

ping_pong: ping_pong.erl
	$(ERLC) $(ERLC_FLAGS) $<

skynet: skynet.erl
	$(ERLC) $(ERLC_FLAGS) $<

run_ping_pong: ping_pong
	$(ERL) $(ERL_FLAGS) -s ping_pong start

# For skynet with 1M+ processes, increase the process limit
run_skynet: skynet
	$(ERL) +P 2000000 $(ERL_FLAGS) -s skynet start

clean:
	rm -f *.beam

.DEFAULT_GOAL := all
