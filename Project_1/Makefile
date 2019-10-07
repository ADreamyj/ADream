CC=g++
bin=Dream
src=Dream.cc
INCLUDE=-Ispeech/
LIB=-ljsoncpp -lcurl -lcrypto

$(bin) : $(src)
	$(CC) -o $@ $^ $(INCLUDE) $(LIB) -std=c++11
.PHONY:clean
clean:
	rm -f $(bin)
