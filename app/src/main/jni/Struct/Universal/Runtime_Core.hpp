#pragma once
#include "Unity_Abstraction.hpp"
#include <atomic>

namespace Universal {

    class Core {
    public:
        static Core& getInstance() {
            static Core instance;
            return instance;
        }

        bool init(const char* assembly, const char* ns, const char* cls, const char* listField) {
            if (isInitialized.load()) return true;

            std::lock_guard<std::mutex> lock(initMutex);
            if (isInitialized.load()) return true;

            LOGD("Initializing Universal ESP Framework...");
            
            Unity::getInstance().initBaseOffsets();
            
            // Resolve offsets específicos do jogo
            listOffset = Resolver::getInstance().resolveField(assembly, ns, cls, listField);
            
            if (listOffset == (size_t)-1) {
                LOGE("Failed to initialize Core: player list field not found.");
                return false;
            }

            isInitialized.store(true);
            LOGD("Universal ESP Framework initialized successfully.");
            return true;
        }

        bool isReady() const {
            return isInitialized.load();
        }

        size_t getListOffset() const {
            return listOffset;
        }

    private:
        std::atomic<bool> isInitialized{false};
        std::mutex initMutex;
        size_t listOffset{0};
        Core() {}
    };
}
