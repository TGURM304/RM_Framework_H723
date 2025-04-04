//
// Created by fish on 2025/1/24.
//

#pragma once

#include <cstdint>

namespace CRC16 {
    static constexpr unsigned offset = sizeof(uint16_t);
    uint16_t calc(const uint8_t *buf, unsigned len, uint16_t crc = 0xffff);
    template <typename T>
    uint16_t calc(const T& res) {
        return calc(reinterpret_cast <const uint8_t *> (&res), sizeof(T) - offset);
    }
    inline bool verify(const uint8_t *res, unsigned len) {
        if(len < offset) return false;
        return *reinterpret_cast<const uint16_t *>(res + len - offset) == calc(res, len - offset);
    }
    template <typename T>
    bool verify(const T& res) {
        if(sizeof(T) < offset) return false;
        return *reinterpret_cast <const uint16_t *> (reinterpret_cast <const uint8_t *> (&res) + sizeof(T) - offset) == calc(res);
    }
    template <typename T>
    void append(T& res) {
        *reinterpret_cast <uint16_t *> (reinterpret_cast <uint8_t *> (&res) + sizeof(T) - offset) = calc(res);
    }
}

namespace CRC8 {
    static constexpr unsigned offset = sizeof(uint8_t);
    uint8_t calc(const uint8_t *buf, unsigned len, uint8_t crc = 0xff);
    template <typename T> uint8_t calc(const T &res) {
        return calc(reinterpret_cast<const uint8_t *>(&res), sizeof(T) - offset);
    }
    inline bool verify(const uint8_t *res, unsigned len) {
        if(len < offset) return false;
        return *(res + len - offset) == calc(res, len - offset);
    }
    template <typename T>
    bool verify(const T& res) {
        if(sizeof(T) < offset) return false;
        return *(reinterpret_cast <const uint8_t *> (&res) + sizeof(T) - offset) == calc(res);
    }
    template <typename T>
    void append(T& res) {
        *(reinterpret_cast <uint8_t *> (&res) + sizeof(T) - offset) = calc(res);
    }
}