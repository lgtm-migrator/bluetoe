#ifndef BLUETOE_BINDINGS_NRF_HPP
#define BLUETOE_BINDINGS_NRF_HPP

#include <bluetoe/meta_types.hpp>
#include <bluetoe/default_pdu_layout.hpp>

namespace bluetoe
{
    /**
     * @brief namespace with nRF51/52 specific configuration options
     */
    namespace nrf
    {
        namespace details {
            struct nrf52_radio_option_meta_type : ::bluetoe::details::binding_option_meta_type {};
            struct sleep_clock_source_meta_type : nrf52_radio_option_meta_type {};
        }

        /**
         * @brief configure the low frequency clock to be sourced out of the high frequency clock
         *
         * The resulting sleep clock accurary is then the accuarcy of your high frequency clock source.
         *
         * @sa bluetoe::link_layer::sleep_clock_accuracy_ppm
         * @sa bluetoe::nrf::sleep_clock_crystal_oscillator
         * @sa bluetoe::nrf::calibrated_sleep_clock
         */
        struct synthesized_sleep_clock
        {
            /** @cond HIDDEN_SYMBOLS */
            using meta_type = details::sleep_clock_source_meta_type;
            /** @endcond */
        };

        /**
         * @brief configure the low frequency clock to be sourced from a crystal oscilator
         *
         * @sa bluetoe::link_layer::sleep_clock_accuracy_ppm
         * @sa bluetoe::nrf::synthesized_sleep_clock
         * @sa bluetoe::nrf::calibrated_sleep_clock
         */
        struct sleep_clock_crystal_oscillator
        {
            /** @cond HIDDEN_SYMBOLS */
            using meta_type = details::sleep_clock_source_meta_type;
            /** @endcond */
        };

        /**
         * @brief configure the low frequency clock to run from the RC oscilator and to be
         *        calibrated, using the high frequency clock.
         *
         * According to the datasheet, the resulting sleep clock accuarcy is then 500ppm.
         * If no sleep clock configuration is given, this is the default.
         *
         * @sa bluetoe::link_layer::sleep_clock_accuracy_ppm
         * @sa bluetoe::nrf::synthesized_sleep_clock
         * @sa bluetoe::nrf::sleep_clock_crystal_oscillator
         */
        struct calibrated_sleep_clock
        {
            /** @cond HIDDEN_SYMBOLS */
            using meta_type = details::sleep_clock_source_meta_type;
            /** @endcond */
        };
    }

    namespace nrf_details
    {
        struct encrypted_pdu_layout : bluetoe::link_layer::details::layout_base< encrypted_pdu_layout >
        {
            /** @cond HIDDEN_SYMBOLS */
            static constexpr std::size_t header_size = sizeof( std::uint16_t );

            using bluetoe::link_layer::details::layout_base< encrypted_pdu_layout >::header;

            static std::uint16_t header( const std::uint8_t* pdu )
            {
                return ::bluetoe::details::read_16bit( pdu );
            }

            static void header( std::uint8_t* pdu, std::uint16_t header_value )
            {
                ::bluetoe::details::write_16bit( pdu, header_value );
            }

            static std::pair< std::uint8_t*, std::uint8_t* > body( const link_layer::read_buffer& pdu )
            {
                assert( pdu.size >= header_size );

                return { &pdu.buffer[ header_size + 1 ], &pdu.buffer[ pdu.size ] };
            }

            static std::pair< const std::uint8_t*, const std::uint8_t* > body( const link_layer::write_buffer& pdu )
            {
                assert( pdu.size >= header_size );

                return { &pdu.buffer[ header_size + 1 ], &pdu.buffer[ pdu.size ] };
            }

            static constexpr std::size_t data_channel_pdu_memory_size( std::size_t payload_size )
            {
                return header_size + payload_size + 1;
            }
            /** @endcond */
        };
    }
}

#endif

