#ifndef BLUETOE_LINK_LAYER_OPTIONS_HPP
#define BLUETOE_LINK_LAYER_OPTIONS_HPP

#include <bluetoe/address.hpp>
#include <bluetoe/connection_details.hpp>
#include <bluetoe/ll_meta_types.hpp>


namespace bluetoe
{
namespace link_layer
{
    namespace details {
        struct device_address_meta_type {};
        struct buffer_sizes_meta_type {};
    }

    /**
     * @brief defines that the device will use a static random address.
     *
     * A static random address is an address that is random, but stays the same every time the device starts.
     * A static random address is the default.
     */
    struct random_static_address
    {
        /**
         * @brief returns true, because this is a random address
         */
        static constexpr bool is_random()
        {
            return true;
        }

        /**
         * @brief takes a scheduled radio and generates a random static address
         */
        template < class Radio >
        static random_device_address address( const Radio& r )
        {
            return address::generate_static_random_address( r.static_random_address_seed() );
        }

        /** @cond HIDDEN_SYMBOLS */
        struct meta_type :
            details::device_address_meta_type,
            details::valid_link_layer_option_meta_type {};
        /** @endcond */
    };

    /**
     * @brief defines a defined static random address
     *
     * The address will be A:B:C:D:E:F
     */
    template < std::uint8_t A, std::uint8_t B, std::uint8_t C, std::uint8_t D, std::uint8_t E, std::uint8_t F >
    struct static_address
    {
        /**
         * @brief returns true, because this is a random address
         */
        static constexpr bool is_random()
        {
            return true;
        }

        /**
         * @brief returns the static, configured address A:B:C:D:E:F
         */
        template < class Radio >
        static random_device_address address( const Radio& )
        {
            static constexpr std::uint8_t addr[] = { F, E, D, C, B, A };
            return ::bluetoe::link_layer::random_device_address( addr );
        }

        /** @cond HIDDEN_SYMBOLS */
        struct meta_type :
            details::device_address_meta_type,
            details::valid_link_layer_option_meta_type {};
        /** @endcond */
    };

    namespace details {
        struct sleep_clock_accuracy_meta_type {};

        template < unsigned long long SleepClockAccuracyPPM >
        struct check_sleep_clock_accuracy_ppm {
            static_assert( SleepClockAccuracyPPM <= 500, "The highest, possible sleep clock accuracy is 500ppm." );

            typedef void type;
        };

    }

    /**
     * @brief defines the sleep clock accuracy of the device hardware.
     *
     * The stack uses the accuracy information to keep the time window where the peripheral listens for radio messages
     * from the central, as small as possible. It's important to determine the real sleep clock accuracy.
     * Giving to large accuracy will leed to not optimal power consumption.
     * To small accuracy will leed to instable connections.
     */
    template < unsigned long long SleepClockAccuracyPPM, typename = typename details::check_sleep_clock_accuracy_ppm< SleepClockAccuracyPPM >::type >
    struct sleep_clock_accuracy_ppm
    {
        /**
         * @brief configured sleep clock accuracy
         */
        static constexpr unsigned accuracy_ppm = static_cast< unsigned >( SleepClockAccuracyPPM );

        /** @cond HIDDEN_SYMBOLS */
        struct meta_type :
            details::sleep_clock_accuracy_meta_type,
            details::valid_link_layer_option_meta_type {};
        /** @endcond */
    };

    /**
     * @brief defines link layer transmit and receive buffer sizes
     */
    template < std::size_t TransmitSize = 61, std::size_t ReceiveSize = 61 >
    struct buffer_sizes
    {
        /** @cond HIDDEN_SYMBOLS */
        struct meta_type :
            details::buffer_sizes_meta_type,
            details::valid_link_layer_option_meta_type {};
        /** @endcond */

        /**
         * configured link layer transmit buffer size in bytes.
         */
        static constexpr std::size_t transmit_buffer_size = TransmitSize;

        /**
         * configured link layer receive buffer size in bytes.
         */
        static constexpr std::size_t receive_buffer_size  = ReceiveSize;
    };
}
}

#endif
