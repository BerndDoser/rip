//============================================================================
// Name        : rip.cpp
// Author      : Bernd Doser <bernd.doser@braintwister.eu>
// Version     : 1.0
// Copyright   : All rights reserved
// Description : Encode WAV to MP3
//============================================================================

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>
#include <lame/lame.h>
#include <sys/stat.h>
#include "dirent.h"
#include "pthread.h"

typedef struct {
    int thread_id;
    std::vector<std::string>::const_iterator *iter_wav_filenames_cur;
    std::vector<std::string>::const_iterator iter_wav_filenames_end;
    int print_level;
    pthread_mutex_t *mutex;
} Data;

void *encoding_worker(void *args)
{
    while (true) {
        Data *data = static_cast<Data*>(args);

        pthread_mutex_lock(data->mutex);
        if (*data->iter_wav_filenames_cur == data->iter_wav_filenames_end) {
            if (data->print_level >= 1) std::cout << "Thread " << data->thread_id << " will leave." << std::endl;
            pthread_mutex_unlock(data->mutex);
            break;
        }
        auto iter_wav_filenames_cur = *data->iter_wav_filenames_cur;
        ++(*data->iter_wav_filenames_cur);
        if (data->print_level >= 1) std::cout << "Thread " << data->thread_id << " encode file " << *iter_wav_filenames_cur << std::endl;
        pthread_mutex_unlock(data->mutex);

        FILE *wav = fopen(iter_wav_filenames_cur->c_str(), "rb");
        if (!wav) throw std::runtime_error("Can't open file " + *iter_wav_filenames_cur);
        fseek(wav, 44, SEEK_SET);

        std::string mp3_filename = iter_wav_filenames_cur->substr(0, iter_wav_filenames_cur->length() - 3) + "mp3";
        FILE *mp3 = fopen(mp3_filename.c_str(), "wb");
        if (!mp3) throw std::runtime_error("Can't open file " + mp3_filename);

        const int pcm_buffer_size = 8192;
        const int mp3_buffer_size = 8192;

        short int pcm_buffer[pcm_buffer_size * 2];
        unsigned char mp3_buffer[mp3_buffer_size];

        lame_t lame = lame_init();
        if (!lame) throw std::runtime_error("Unable to initialize MP3");
        lame_set_quality(lame, 2);
        if (lame_init_params(lame) < 0) throw std::runtime_error("Unable to initialize MP3 parameters");

        int read, write;
        do {
            read = fread(pcm_buffer, 2*sizeof(short int), pcm_buffer_size, wav);
            if (read == 0)
                write = lame_encode_flush(lame, mp3_buffer, mp3_buffer_size);
            else
                write = lame_encode_buffer_interleaved(lame, pcm_buffer, read, mp3_buffer, mp3_buffer_size);
            fwrite(mp3_buffer, write, 1, mp3);
        } while (read != 0);

        lame_close(lame);
        fclose(mp3);
        fclose(wav);
    }
    return NULL;
}

std::vector<std::string> get_all_wav_filenames(std::string const& dirname)
{
    std::vector<std::string> wav_files;
    DIR *dir = opendir(dirname.c_str());
    if (!dir) throw std::runtime_error("Can not open directory " + dirname);
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        //if(!S_ISREG(ent->d_type)) continue;
        std::string wav_file(ent->d_name);
        if (wav_file.length() < 4 or wav_file.substr(wav_file.length() - 4, wav_file.length()) != ".wav") continue;
#ifdef WIN32
        wav_files.push_back(dirname + "\\" + wav_file);
#else
        wav_files.push_back(dirname + "/" + wav_file);
#endif
    }
    closedir(dir);
    return wav_files;
}

int main(int argc, char* argv[])
{
    try {
        std::cout << "Let's rip" << std::endl;
        if (argc != 2) {
            std::cerr << "USAGE: " << argv[0] << " <path>" << std::endl;
            return 1;
        }

        const int print_level = 1;
        std::vector<std::string> wav_filenames = get_all_wav_filenames(argv[1]);

        const int nb_threads = std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 1;
        if (print_level >= 1) std::cout << "Number of threads: " << nb_threads << std::endl;
        pthread_t *threads = new pthread_t[nb_threads];
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

        // create thread data
        Data *data = new Data[nb_threads];

        std::vector<std::string>::const_iterator iter_wav_filenames_cur = wav_filenames.begin();
        for (int i = 0; i < nb_threads; ++i) {
            data[i].thread_id = i;
            data[i].iter_wav_filenames_cur = &iter_wav_filenames_cur;
            data[i].iter_wav_filenames_end = wav_filenames.end();
            data[i].print_level = print_level;
            data[i].mutex = &mutex;
        }

        // create worker threads
        for (int i = 0; i < nb_threads; ++i) {
            pthread_create(&threads[i], NULL, encoding_worker, (void*)&data[i]);
        }

        // synchronize
        for (int i = 0; i < nb_threads; ++i) {
            if(pthread_join(threads[i], NULL)) throw std::runtime_error("thread join error");
        }

    } catch (std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "All done." << std::endl;
    return 0;
}
