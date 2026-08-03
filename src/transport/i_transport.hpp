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
     * @brief Checks whether this transport currently has a live, working connection.
     * 
     * Semantics differ per implementation: for USB, this reflects an
     * ongoing heartbeat from the host; for WiFi, this reflects the
     * underlying TCP socket state. ConnectionManager uses this to
     * arbitrate between multiple available transports.
     * 
     * @return true if the transport is currently ready to send data.
     */
    virtual bool isReady() const = 0;

    /*
     * @brief Enables or disables this transport's discovery beacon.
     *
     * While enabled, an implementation may periodically announce its
     * presence (e.g. an identity packet) so a host can find it before a
     * connection exists. ConnectionManager disables this on whichever
     * transport is currently active, since only one connected device is
     * supported at a time and continuing to announce would be pointless
     * (and could interleave beacon bytes with real traffic). Implementations
     * for which this concept doesn't apply may treat this as a no-op.
     *
     * @param enabled true to allow beacon announcements, false to suppress them.
     */
    virtual void setBeaconEnabled(bool enabled) = 0;

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
