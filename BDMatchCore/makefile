CXX = clang++
CXXFLAGS = -Wall -O2 -D_CLI_ -D_NO_SVML_ -std=c++17 -mavx2 -mfma

ifeq ($(CXX), clang++)
CXXFLAGS += -stdlib=libc++
STDLIB = -lc++
else ifeq ($(CXX), g++)
STDLIB = 
endif

headers = headers/

# ==== FFmpeg dependencies ====
#verision: 4.2.2
#In ubuntu 18.04, it can be fetched by:
#sudo apt install software-properties-common
#sudo add-apt-repository --yes ppa:jonathonf/ffmpeg-4
#sudo apt update
#sudo apt install ffmpeg
FFmpeg_include = include
FFmpeg_headers = $(FFmpeg_include)/libavcodec/avcodec.h \
		 $(FFmpeg_include)/libavformat/avformat.h \
		 $(FFmpeg_include)/libavutil/imgutils.h \
		 $(FFmpeg_include)/libavutil/samplefmt.h \
		 $(FFmpeg_include)/libavutil/timestamp.h \
		 $(FFmpeg_include)/libavutil/frame.h \
		 $(FFmpeg_include)/libavutil/mem.h \
		 $(FFmpeg_include)/libswscale/swscale.h \
		 $(FFmpeg_include)/libavutil/channel_layout.h \
		 $(FFmpeg_include)/libavutil/md5.h \
		 $(FFmpeg_include)/libavutil/opt.h \
		 $(FFmpeg_include)/libavfilter/avfilter.h \
		 $(FFmpeg_include)/libavfilter/buffersink.h \
		 $(FFmpeg_include)/libavfilter/buffersrc.h \
		 $(FFmpeg_include)/libswresample/swresample.h
		 
FFmpeg_libs = -l:libavcodec.so.58 -l:libavformat.so.58 -l:libavutil.so.56 -l:libswresample.so.3 -l:libswscale.so.5

# ==== fftw dependencies ====
#verision: 3.3.7 or 3.3.8
#In ubuntu 18.04, it can be fetched by:
#sudo apt-get install libfftw3-dev
fftw_headers =  fftw3.h

fftw_libs = -lfftw3

all: bdmatch

clean:
	rm -f *.o bdmatch

# ==== Various Low-level Stuff =====
language_pack.o: $(headers)targetver.h $(headers)language_pack.h
	$(CXX) $(CXXFLAGS) -c -o language_pack.o language_pack.cpp

datastruct.o: $(headers)targetver.h $(headers)datastruct.h
	$(CXX) $(CXXFLAGS) -c -o datastruct.o datastruct.cpp

# ==== Decoder =====
decoder.o: $(headers)language_pack.h $(headers)datastruct.h $(headers)multithreading.h \
		   $(FFmpeg_headers) $(fftw_headers) $(headers)decoder.h
	$(CXX) $(CXXFLAGS) -c -o decoder.o decoder.cpp

# ==== Matching =====
matching.o: $(headers)language_pack.h $(headers)datastruct.h $(headers)multithreading.h \
			$(headers)matching.h
	$(CXX) $(CXXFLAGS) -c -o matching.o matching.cpp

# ==== BDMatchCore =====
bdmatchcore.o: $(headers)language_pack.h $(headers)multithreading.h \
			   $(headers)decoder.h $(headers)matching.h \
			   $(headers)BDMatchCore.h
	$(CXX) $(CXXFLAGS) -c -o bdmatchcore.o BDMatchCore.cpp

# ==== CLI =====
bdmatch.o:
	$(CXX) $(CXXFLAGS) -c -o bdmatch.o CLI.cpp

bdmatch: bdmatch.o bdmatchcore.o matching.o decoder.o \
		 datastruct.o language_pack.o
	$(CXX) -o bdmatch \
	bdmatch.o bdmatchcore.o matching.o decoder.o \
	datastruct.o language_pack.o \
	-lc -lm -lpthread $(FFmpeg_libs) $(fftw_libs) $(STDLIB)