#define BOOST_TEST_MODULE
#include <boost/test/included/unit_test.hpp>

#include "test_sm.hpp"
#include "test_servers.hpp"

static const std::initializer_list< std::uint8_t > no_lesc_pairing_request = {
    0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00
};

static const std::initializer_list< std::uint8_t > lesc_pairing_request = {
    0x01, 0x00, 0x00, 0x08, 0x07, 0x00, 0x00
};

using no_security_sm = test::security_manager< bluetoe::no_security_manager, test::lesc_security_functions, 27 >;

BOOST_FIXTURE_TEST_CASE( no_security_manager_no_pairing, no_security_sm )
{
    expected(
        no_lesc_pairing_request,
        {
            0x05, 0x05
        }
    );
}

BOOST_FIXTURE_TEST_SUITE( legacy_pairing, test::legacy_security_manager<> )

    BOOST_AUTO_TEST_CASE( by_default_no_oob_no_lesc )
    {
        expected(
            no_lesc_pairing_request,
            {
                0x02,   // response
                0x03,   // NoInputNoOutput
                0x00,   // OOB Authentication data not present
                0x00,   // Bonding, MITM = 0, SC = 0, Keypress = 0
                0x10,   // Maximum Encryption Key Size
                0x00,   // LinkKey
                0x00    // LinkKey
            }
        );
    }

    BOOST_AUTO_TEST_CASE( no_lesc_event_when_the_initiater_is_asking_for_it )
    {
        expected(
            lesc_pairing_request,
            {
                0x02,   // response
                0x03,   // NoInputNoOutput
                0x00,   // OOB Authentication data not present
                0x00,   // Bonding, MITM = 0, SC = 0, Keypress = 0
                0x10,   // Maximum Encryption Key Size
                0x00,   // LinkKey
                0x00    // LinkKey
            }
        );
    }

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE( lesc_pairing, test::lesc_security_manager<> )

    BOOST_AUTO_TEST_CASE( by_default_no_oob_but_lesc )
    {
        expected(
            lesc_pairing_request,
            {
                0x02,   // response
                0x03,   // NoInputNoOutput
                0x00,   // OOB Authentication data not present
                0x08,   // Bonding, MITM = 0, SC = 1, Keypress = 0
                0x10,   // Maximum Encryption Key Size
                0x00,   // LinkKey
                0x00    // LinkKey
            }
        );
    }

BOOST_AUTO_TEST_SUITE_END()

/**
 * SM/SLA/PROT/BV-02-C [SMP Time Out – IUT Responder]
 *
 * Verify that the IUT responder disconnects the link if pairing does not follow Pairing
 * Feature Exchange within 30 seconds after receiving Pairing Request command.
 */

/**
 * SM/SLA/JW/BV-02-C [Just Works IUT Responder – Success]
 *
 * Verify that the IUT is able to perform the Just Works pairing procedure correctly when acting as slave, responder.
 */
BOOST_FIXTURE_TEST_CASE( Just_Works_IUT_Responder__Success, test::legacy_security_manager<> )
{
    expected(
        {
            0x01,       // Pairing request
            0x03,       // NoInputNoOutput
            0x00,       // OOB Authentication data not present
            0x00,       // No Bonding, No MITM
            0x10,       // Maximum Encryption Key Size
            0x00,       // Initiator Key Distribution
            0x00        // Responder Key Distribution
        },
        {
            0x02,       // Pairing Response
            0x03,       // NoInputNoOutput
            0x00,       // OOB Authentication data not present
            0x00,       // No Bonding, MITM = 0, SC = 0, Keypress = 0
            0x10,       // Maximum Encryption Key Size
            0x00,       // Initiator Key Distribution
            0x00        // Responder Key Distribution
        }
    );
}

/**
 * SM/SLA/JW/BI-03-C [Just Works IUT Responder – Handle AuthReq flag RFU correctly]
 *
 * Verify that the IUT is able to perform the Just Works pairing procedure when receiving additional
 * bits set in the AuthReq flag. Reserved For Future Use bits are correctly handled when acting as
 * slave, responder.
 */

/**
 * SM/SLA/JW/BI-02-C [Just Works, IUT Responder – Failure]
 *
 * Verify that the IUT handles just works pairing failure as responder correctly.
 */

/**
 * SM/SLA/PKE/BV-02-C (Passkey Entry, IUT Responder – Success)
 *
 * Verify that the IUT performs the Passkey Entry pairing procedure correctly as responder.
 */

/**
 * SM/SLA/PKE/BV-05-C [Passkey Entry, IUT Responder – Lower Tester has insufficient security for Passkey Entry]
 *
 * Verify that the IUT that supports the Passkey Entry pairing procedure as responder correctly
 * handles an initiator with insufficient security to result in an Authenticated key, yielding an
 * unauthenticated key.
 */

/**
 * SM/SLA/PKE/BI-03-C [Passkey Entry, IUT Responder – Failure on Initiator Side]
 *
 * Verify that the IUT handles the invalid passkey entry pairing procedure correctly as responder.
 */

struct oob_cb_t {
    static const std::array< std::uint8_t, 16 > oob_data;

    std::pair< bool, std::array< std::uint8_t, 16 > > sm_oob_authentication_data(
        const bluetoe::link_layer::device_address& address )
    {
        obb_requesting_remote_address = address;

        return { true, oob_data };
    }

    bluetoe::link_layer::device_address obb_requesting_remote_address;
} oob_cb;

const std::array< std::uint8_t, 16 > oob_cb_t::oob_data = {{
        0xF1, 0x50, 0xA0, 0xAE,
        0xB7, 0xAA, 0xBA, 0xC8,
        0x19, 0x22, 0xB6, 0x15,
        0x4C, 0x23, 0x94, 0x7A
    }};

using legacy_security_manager_with_oob = test::legacy_security_manager< 23, bluetoe::oob_authentication_callback< oob_cb_t, oob_cb > >;

/**
 * SM/SLA/OOB/BV-02-C [IUT Responder – Both sides have OOB data – Success]
 *
 * Verify that the IUT performs the OOB pairing procedure correctly as responder.
 */
BOOST_FIXTURE_TEST_CASE( IUT_Responder_Both_sides_have_OOB_data_Success, legacy_security_manager_with_oob )
{
    expected(
        {
            0x01,       // Pairing request
            0x03,       // NoInputNoOutput
            0x01,       // OOB Authentication data from remote device present
            0x00,       // No Bonding, No MITM
            0x10,       // Maximum Encryption Key Size
            0x00,       // Initiator Key Distribution
            0x00        // Responder Key Distribution
        },
        {
            0x02,       // Pairing Response
            0x03,       // NoInputNoOutput
            0x01,       // OOB Authentication data from remote device present
            0x00,       // No Bonding, MITM = 0, SC = 0, Keypress = 0
            0x10,       // Maximum Encryption Key Size
            0x00,       // Initiator Key Distribution
            0x00        // Responder Key Distribution
        }
    );

    BOOST_CHECK_EQUAL( oob_cb.obb_requesting_remote_address,
        bluetoe::link_layer::random_device_address({ 0xa6, 0xa5, 0xa4, 0xa3, 0xa2, 0xa1 }) );

    expected_pairing_confirm_with_tk( oob_cb_t::oob_data );
}

/**
 * SM/SLA/OOB/BV-04-C [IUT Responder – Only IUT has OOB data – Success]
 *
 * Verify that the IUT performs the pairing procedure correctly as responder if only the
 * IUT has OOB data.
 */

/**
 * SM/SLA/OOB/BV-06-C [IUT Responder – Only Lower Tester has OOB data – Success]
 *
 * Verify that the IUT performs the pairing procedure correctly as responder if only the Lower Tester has OOB data.
 */

/**
 * SM/SLA/OOB/BV-08-C [IUT Responder – Only Lower Tester has OOB data – Lower Tester also supports Just Works]
 *
 * Verify that the IUT performs the pairing procedure correctly as responder if only the Lower Tester has OOB
 * data and supports the Just Works pairing method
 */

/**
 * SM/SLA/OOB/BV-10-C [IUT Responder – Only IUT has OOB data – Lower Tester also supports Just Works]
 *
 * Verify that the IUT performs the pairing procedure correctly as responder if only the IUT has
 * OOB data and the Lower Tester supports the Just Works pairing method.
 */

/**
 * SM/SLA/OOB/BI-02-C [IUT Responder – Both sides have different OOB data – Failure]
 *
 * Verify that the IUT responds to OOB pairing procedure and handles the failure correctly.
 */
