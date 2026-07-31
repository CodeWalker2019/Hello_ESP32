/**
 * @file i_transport.hpp
 * @brief Abstract interface defining the transport layer contract for embedded systems.
 * 
 * This interface provides a polymorphic abstraction for various communication 
 * mediums (e.g., USB, UART, Wi-Fi, BLE), allowing higher-level components 
 * like ConnectionManager to interact with them uniformly.
 */

#ifndef I_TRANSPORT_HPP
#define I_TRANSPORT_HPP

#include <cstdint>
#include <cstddef>

class ITransport {
public:
    /**
     * @brief Virtual destructor.
     * 
     * Essential for polymorphic base classes. Ensures that when a derived object 
     * (e.g., UsbTransport) is destroyed via a base class pointer (ITransport*), 
     * the derived class's destructor is called first, preventing hardware resource leaks.
     */
    virtual ~ITransport() = default;

    /**
     * @brief Initializes the transport medium and associated hardware/peripherals.
     * 
     * Returns a status flag because initialization can fail (e.g., a Wi-Fi module 
     * failing to associate or a peripheral being busy). This allows the manager 
     * to handle startup errors gracefully.
     * 
     * @return true if initialization succeeded, false otherwise.
     */
    virtual bool init() = 0;

    /**
     * @brief Transmits a raw buffer of data over the transport.
     * 
     * Uses a raw pointer and length rather than dynamic containers (like std::vector) 
     * to completely avoid heap allocations and memory fragmentation in hot paths 
     * (e.g., high-frequency telemetry streaming at 100Hz).
     * 
     * @param data Pointer to the data buffer to transmit.
     * @param len Number of bytes to send.
     * @return size_t Number of bytes successfully sent.
     */
    virtual size_t send(const uint8_t* data, size_t len) = 0;
};

#endif // I_TRANSPORT_HPP