/*
 * ntp.h
 *
 *      Author: Matheus
 */

#ifndef NTP_H_
#define NTP_H_

//http://stackoverflow.com/questions/13011532/get-time-date-from-server-with-sntpwindows-c

#define ReverseEndianInt(x) ((x) = \
    ((x)&0xff000000) >> 24 |\
    ((x)&0x00ff0000) >> 8 |\
    ((x)&0x0000ff00) << 8 |\
    ((x)&0x000000ff) << 24)

/**
 * NTP Fixed-Point Timestamp Format.
 * From [RFC 5905](http://tools.ietf.org/html/rfc5905).
 */
struct Timestamp
{
    unsigned int seconds; /**< Seconds since Jan 1, 1900. */
    unsigned int fraction; /**< Fractional part of seconds. Integer number of 2^-32 seconds. */

    /**
     * Reverses the Endianness of the timestamp.
     * Network byte order is big endian, so it needs to be switched before
     * sending or reading.
     */
    void ReverseEndian() {
        ReverseEndianInt(seconds);
        ReverseEndianInt(fraction);
    }

    /**
     * Convert to time_t.
     * Returns the integer part of the timestamp in unix time_t format,
     * which is seconds since Jan 1, 1970.
     */
    time_t to_time_t()
    {
        return (seconds - ((70 * 365 + 17) * 86400))&0x7fffffff;
    }
};

/**
 * A Network Time Protocol Message.
 * From [RFC 5905](http://tools.ietf.org/html/rfc5905).
 */
struct NTPMessage
{
    unsigned int mode :3; /**< Mode of the message sender. 3 = Client, 4 = Server */
    unsigned int version :2; /**< Protocol version. Should be set to 3. */
    unsigned int leap :2; /**< Leap seconds warning. See the [RFC](http://tools.ietf.org/html/rfc5905#section-7.3) */
    unsigned char stratum; /**< Servers between client and physical timekeeper. 1 = Server is Connected to Physical Source. 0 = Unknown. */
    unsigned char poll; /**< Max Poll Rate. In log2 seconds. */
    unsigned char precision; /**< Precision of the clock. In log2 seconds. */
    unsigned int sync_distance; /**< Round-trip to reference clock. NTP Short Format. */
    unsigned int drift_rate; /**< Dispersion to reference clock. NTP Short Format. */
    unsigned char ref_clock_id[4]; /**< Reference ID. For Stratum 1 devices, a 4-byte string. For other devices, 4-byte IP address. */
    Timestamp ref; /**< Reference Timestamp. The time when the system clock was last updated. */
    Timestamp orig; /**< Origin Timestamp. Send time of the request. Copied from the request. */
    Timestamp rx; /**< Recieve Timestamp. Reciept time of the request. */
    Timestamp tx; /**< Transmit Timestamp. Send time of the response. If only a single time is needed, use this one. */


    /**
     * Reverses the Endianness of all the timestamps.
     * Network byte order is big endian, so they need to be switched before
     * sending and after reading.
     *
     * Maintaining them in little endian makes them easier to work with
     * locally, though.
     */
    void ReverseEndian() {
        ref.ReverseEndian();
        orig.ReverseEndian();
        rx.ReverseEndian();
        tx.ReverseEndian();
    }

    /**
     * Zero all the values.
     */
    void clear()
    {
        memset(this, 0, sizeof(*this));
    }
};

#endif /* NTP_H_ */
