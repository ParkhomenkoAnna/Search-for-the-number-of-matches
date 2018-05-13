compiler	=gcc
compflags	=-g -Wall -pthread
path		=bin
obj		=	$(path)/mysearch.o

default : checkdir $(path)/mysearch

checkdir :
	@if [ ! -d $(path) ]; then mkdir $(path); fi

$(path)/mysearch : $(obj)
	$(compiler) $(compflags) -o $(path)/mysearch $(obj)

$(path)/%.o : %.c
	$(compiler) $(compflags) -c $< -o $@

clean :
	rm -f $(path)/*