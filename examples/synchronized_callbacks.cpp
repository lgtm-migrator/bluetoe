/*
 * Example of callbacks that are synchronized to the connection event.
 *
 * API Documentation: link_layer/include/bluetoe/connection_event_callback.hpp
 */

#include <bluetoe/server.hpp>
#include <bluetoe/device.hpp>

#include <bluetoe/connection_event_callback.hpp>

#include <nrf.h>

using namespace bluetoe;

// LED1 on a nRF52 eval board
static constexpr int io_pin = 13;

// GPIO
static constexpr int io_event = 14;

static void set_io( int pin, bool value )
{
    if ( value )
    {
        NRF_GPIO->OUTSET = 1 << pin;
    }
    else
    {
        NRF_GPIO->OUTCLR = 1 << pin;
    }
}

static std::uint8_t io_pin_write_handler( bool state )
{
    // on an nRF52 eval board, the pin is connected to the LED's cathode, this inverts the logic.
    set_io( io_pin, !state );

    return error_codes::success;
}

/*
 * Type as set of callbacks
 */
struct callback_handler_t {
    /*
     * User defined type to keep connection releated informations
     */
    struct connection {
        connection() : pin( false ) {}
        bool pin;
    };

    unsigned ll_synchronized_callback( unsigned, connection& con )
    {
        set_io( io_event, con.pin );
        con.pin = !con.pin;

        // Number of calls to skip
        return 0;
    }
};

using blinky_server = server<
    service<
        service_uuid< 0xC11169E1, 0x6252, 0x4450, 0x931C, 0x1B43A318783B >,
        characteristic<
            free_write_handler< bool, io_pin_write_handler >
        >
    >
>;

// Instance of callback handler
callback_handler_t callback_handler;

device<
    blinky_server,
    link_layer::buffer_sizes< 200, 200 >,
    bluetoe::link_layer::synchronized_connection_event_callback<
        callback_handler_t, callback_handler, 4000, -100 >
> gatt_srv;

int main()
{
    // Init GPIO pin
    NRF_GPIO->PIN_CNF[ io_pin ] =
        ( GPIO_PIN_CNF_DRIVE_S0H1 << GPIO_PIN_CNF_DRIVE_Pos ) |
        ( GPIO_PIN_CNF_DIR_Output << GPIO_PIN_CNF_DIR_Pos );

    for ( ;; )
        gatt_srv.run();
}
