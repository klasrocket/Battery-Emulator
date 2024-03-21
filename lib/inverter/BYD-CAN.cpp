#include "BYD-CAN.h"

#include <ESP32CAN.h>

/* Do not change code below unless you are sure what you are doing */
static unsigned long previousMillis2s = 0;   // will store last time a 2s CAN Message was send
static unsigned long previousMillis10s = 0;  // will store last time a 10s CAN Message was send
static unsigned long previousMillis60s = 0;  // will store last time a 60s CAN Message was send
static const int interval2s = 2000;          // interval (ms) at which send CAN Messages
static const int interval10s = 10000;        // interval (ms) at which send CAN Messages
static const int interval60s = 60000;        // interval (ms) at which send CAN Messages
static uint8_t char1_151 = 0;
static uint8_t char2_151 = 0;
static uint8_t char3_151 = 0;
static uint8_t char4_151 = 0;
static uint8_t char5_151 = 0;
static uint8_t char6_151 = 0;
static uint8_t char7_151 = 0;

//Startup messages



twai_message_t BYD_250 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x250,
    .data_length_code = 8,
    .data = {0x03, 0x16, 0x00, 0x66, (uint8_t)((BATTERY_WH_MAX / 100) >> 8), (uint8_t)(BATTERY_WH_MAX / 100), 0x02,0x09},
}; 
//const CAN_frame_t BYD_250 = {
//    .FIR = {.B =
//                {
//                    .DLC = 8,
//                    .FF = CAN_frame_std,
//                }},
//    .MsgID = 0x250,
//    .data = {0x03, 0x16, 0x00, 0x66, (uint8_t)((BATTERY_WH_MAX / 100) >> 8), (uint8_t)(BATTERY_WH_MAX / 100), 0x02,
//             0x09}};  //3.16 FW , Capacity kWh byte4&5 (example 24kWh = 240)


twai_message_t BYD_290 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x290,
    .data_length_code = 8,
    .data = {0x06, 0x37, 0x10, 0xD9, 0x00, 0x00, 0x00, 0x00},
}; 

twai_message_t BYD_2D0 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x2D0,
    .data_length_code = 8,
    .data = {0x00, 0x42, 0x59, 0x44, 0x00, 0x00, 0x00, 0x00},
};                            

twai_message_t BYD_3D0_0 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x3D0,
    .data_length_code = 8,
    .data = {0x00, 0x42, 0x61, 0x74, 0x74, 0x65, 0x72, 0x79},
};                            
  //Battery
twai_message_t BYD_3D0_1 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x3D0,
    .data_length_code = 8,
    .data = {0x01, 0x2D, 0x42, 0x6F, 0x78, 0x20, 0x50, 0x72},
};                              
 //-Box Pr
twai_message_t BYD_3D0_2 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x3D0,
    .data_length_code = 8,
    .data = {0x02, 0x65, 0x6D, 0x69, 0x75, 0x6D, 0x20, 0x48},
};                                
  //emium H
twai_message_t BYD_3D0_3 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x3D0,
    .data_length_code = 8,
    .data = {0x03, 0x56, 0x53, 0x00, 0x00, 0x00, 0x00, 0x00},
};                               
  //VS
twai_message_t BYD_110 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x110,
    .data_length_code = 8,
    .data = {0x01, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};                              
//Actual content messages

twai_message_t BYD_150 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x150,
    .data_length_code = 8,
    .data = {0x00, 0x00, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00},
};                       

twai_message_t BYD_190 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x190,
    .data_length_code = 8,
    .data = {0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00},
};                       

twai_message_t BYD_1D0 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x1D0,
    .data_length_code = 8,
    .data = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x08},
};                       

twai_message_t BYD_210 = {
    // Message type and format settings
    .extd = 0,              // Standard vs extended format

    // Message ID and payload
    .identifier = 0x210,
    .data_length_code = 8,
    .data = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};                      






static uint16_t discharge_current = 0;
static uint16_t charge_current = 0;
static int16_t temperature_average = 0;
static uint16_t inverter_voltage = 0;
static uint16_t inverter_SOC = 0;
static long inverter_timestamp = 0;
static bool initialDataSent = 0;

void update_values_can_byd() {  //This function maps all the values fetched from battery CAN to the correct CAN messages
  //Calculate values
  charge_current = ((system_max_charge_power_W * 10) /
                    system_max_design_voltage_dV);  //Charge power in W , max volt in V+1decimal (P=UI, solve for I)
  //The above calculation results in (30 000*10)/3700=81A
  charge_current = (charge_current * 10);  //Value needs a decimal before getting sent to inverter (81.0A)
  if (charge_current > MAXCHARGEAMP) {
    charge_current = MAXCHARGEAMP;  //Cap the value to the max allowed Amp. Some inverters cannot handle large values.
  }

  discharge_current = ((system_max_discharge_power_W * 10) /
                       system_max_design_voltage_dV);  //Charge power in W , max volt in V+1decimal (P=UI, solve for I)
  //The above calculation results in (30 000*10)/3700=81A
  discharge_current = (discharge_current * 10);  //Value needs a decimal before getting sent to inverter (81.0A)
  if (discharge_current > MAXDISCHARGEAMP) {
    discharge_current =
        MAXDISCHARGEAMP;  //Cap the value to the max allowed Amp. Some inverters cannot handle large values.
  }

  temperature_average = ((system_temperature_max_dC + system_temperature_min_dC) / 2);

  //Map values to CAN messages
  //Maxvoltage (eg 400.0V = 4000 , 16bits long)
  BYD_110.data[0] = (system_max_design_voltage_dV >> 8);
  BYD_110.data[1] = (system_max_design_voltage_dV & 0x00FF);
  //Minvoltage (eg 300.0V = 3000 , 16bits long)
  BYD_110.data[2] = (system_min_design_voltage_dV >> 8);
  BYD_110.data[3] = (system_min_design_voltage_dV & 0x00FF);
  //Maximum discharge power allowed (Unit: A+1)
  BYD_110.data[4] = (discharge_current >> 8);
  BYD_110.data[5] = (discharge_current & 0x00FF);
  //Maximum charge power allowed (Unit: A+1)
  BYD_110.data[6] = (charge_current >> 8);
  BYD_110.data[7] = (charge_current & 0x00FF);

  //SOC (100.00%)
  BYD_150.data[0] = (system_scaled_SOC_pptt >> 8);
  BYD_150.data[1] = (system_scaled_SOC_pptt & 0x00FF);
  //StateOfHealth (100.00%)
  BYD_150.data[2] = (system_SOH_pptt >> 8);
  BYD_150.data[3] = (system_SOH_pptt & 0x00FF);
  //Maximum charge power allowed (Unit: A+1)
  BYD_150.data[4] = (charge_current >> 8);
  BYD_150.data[5] = (charge_current & 0x00FF);
  //Maximum discharge power allowed (Unit: A+1)
  BYD_150.data[6] = (discharge_current >> 8);
  BYD_150.data[7] = (discharge_current & 0x00FF);

  //Voltage (ex 370.0)
  BYD_1D0.data[0] = (system_battery_voltage_dV >> 8);
  BYD_1D0.data[1] = (system_battery_voltage_dV & 0x00FF);
  //Current (ex 81.0A)
  BYD_1D0.data[2] = (system_battery_current_dA >> 8);
  BYD_1D0.data[3] = (system_battery_current_dA & 0x00FF);
  //Temperature average
  BYD_1D0.data[4] = (temperature_average >> 8);
  BYD_1D0.data[5] = (temperature_average & 0x00FF);

  //Temperature max
  BYD_210.data[0] = (system_temperature_max_dC >> 8);
  BYD_210.data[1] = (system_temperature_max_dC & 0x00FF);
  //Temperature min
  BYD_210.data[2] = (system_temperature_min_dC >> 8);
  BYD_210.data[3] = (system_temperature_min_dC & 0x00FF);

#ifdef DEBUG_VIA_USB
  if (char1_151 != 0) {
    Serial.print("Detected inverter: ");
    Serial.print((char)char1_151);
    Serial.print((char)char2_151);
    Serial.print((char)char3_151);
    Serial.print((char)char4_151);
    Serial.print((char)char5_151);
    Serial.print((char)char6_151);
    Serial.println((char)char7_151);
  }
#endif
}

void receive_can_byd(twai_message_t rx_frame) {
  switch (rx_frame.identifier) {
    case 0x151:  //Message originating from BYD HVS compatible inverter. Reply with CAN identifier!
      if (rx_frame.data[0] & 0x01) {  //Battery requests identification
        send_intial_data();
      } else {  // We can identify what inverter type we are connected to
        char1_151 = rx_frame.data[1];
        char2_151 = rx_frame.data[2];
        char3_151 = rx_frame.data[3];
        char4_151 = rx_frame.data[4];
        char5_151 = rx_frame.data[5];
        char6_151 = rx_frame.data[6];
        char7_151 = rx_frame.data[7];
      }
      break;
    case 0x091:
      inverter_voltage = ((rx_frame.data[1] << 8) | rx_frame.data[0]) * 0.1;
      break;
    case 0x0D1:
      inverter_SOC = ((rx_frame.data[1] << 8) | rx_frame.data[0]) * 0.1;
      break;
    case 0x111:
      inverter_timestamp = ((rx_frame.data[3] << 24) | (rx_frame.data[2] << 16) | (rx_frame.data[1] << 8) |
                            rx_frame.data[0]);
      break;
    default:
      break;
  }
}

void send_can_byd() {
  unsigned long currentMillis = millis();
  // Send initial CAN data once on bootup
  if (!initialDataSent) {
    send_intial_data();
    initialDataSent = 1;
  }

  // Send 2s CAN Message
  if (currentMillis - previousMillis2s >= interval2s) {
    previousMillis2s = currentMillis;

    ESP32Can.CANWriteFrame(&BYD_110);
      Serial.println("send can byd 110");

  }
  // Send 10s CAN Message
  if (currentMillis - previousMillis10s >= interval10s) {
    previousMillis10s = currentMillis;

    ESP32Can.CANWriteFrame(&BYD_150);
    ESP32Can.CANWriteFrame(&BYD_1D0);
    ESP32Can.CANWriteFrame(&BYD_210);
          Serial.println("send can byd 150,1d0,210");

  }
  //Send 60s message
  if (currentMillis - previousMillis60s >= interval60s) {
    previousMillis60s = currentMillis;

    ESP32Can.CANWriteFrame(&BYD_190);

//     twai_message_t BYD_250;
// BYD_250.extd = 0;
// BYD_250.data_length_code = 8;
// BYD_250.identifier = 0x1F2;
// BYD_250.data[0] = 0x10;
// BYD_250.data[1] = 0x64;
// BYD_250.data[2] = 0x00;
// BYD_250.data[3] = 0xB0;
// BYD_250.data[4] = 0x00;
// BYD_250.data[5] = 0x1E;
// BYD_250.data[6] = 0x00;
// BYD_250.data[7] = 0x8F;

//     ESP32Can.CANWriteFrame(&BYD_250); /* send the CAN message */

              Serial.println("send can byd 190");

  }
}

void send_intial_data() {
  ESP32Can.CANWriteFrame(&BYD_250);
  ESP32Can.CANWriteFrame(&BYD_290);
  ESP32Can.CANWriteFrame(&BYD_2D0);
  ESP32Can.CANWriteFrame(&BYD_3D0_0);
  ESP32Can.CANWriteFrame(&BYD_3D0_1);
  ESP32Can.CANWriteFrame(&BYD_3D0_2);
  ESP32Can.CANWriteFrame(&BYD_3D0_3);
}