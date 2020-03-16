CC = gcc -Wall -std=c11 -ggdb

all: test clean

test:
		$(CC) -o main main.c -O2

		echo "generating tests for sort"
		echo "" > sort_test_result.txt
		for l in 4 512 4096 8192; \
		do \
		  	for a in 3000 10000 60000 120000; \
		  	do \
		  	  	./main generate test_file $$a $$l; \
		  	  	cp test_file test_file_sys; \
		  	  	cp test_file test_file_lib; \
		  	  	rm test_file; \
		  	  	echo "test for file with $$a records of length $$l" >> sort_test_result.txt; \
		  	  	echo "sys" >> sort_test_result.txt; \
		  	  	./main sort test_file_sys $$a $$l sys >> sort_test_result.txt; \
		  	  	echo "lib" >> sort_test_result.txt; \
		  	  	./main sort test_file_lib $$a $$l lib >> sort_test_result.txt; \
		  	  	rm test_file_sys test_file_lib; \
		  	done; \
		done

		echo "generating tests for copy"
		echo "" > copy_test_result.txt
		for l in 4 512 4096 8192; \
		do \
		  	for a in 60000 120000 360000 500000; \
		  	do \
		  	  	./main generate test_file $$a $$l; \
		  	  	cp test_file test_file_sys; \
		  	  	cp test_file test_file_lib; \
		  	  	rm test_file; \
		  	  	echo "test for file with $$a records of length $$l" >> copy_test_result.txt; \
		  	  	echo "sys" >> copy_test_result.txt; \
		  	  	./main copy test_file_sys test_file_sys_tmp $$a $$l sys >> copy_test_result.txt; \
		  	  	echo "lib" >> copy_test_result.txt; \
		  	  	./main copy test_file_lib test_file_lib_tmp $$a $$l lib >> copy_test_result.txt; \
		  	  	rm test_file_sys test_file_lib test_file_lib_tmp test_file_sys_tmp; \
		  	done; \
		done

clean:
		rm -f *.o
