
## Challenge Requirements
Design and implement a protocol that allows “pairing” of two fictional devices similarly to how a headset might be paired with a phone. The protocol should include features such as:
A hello message for initialization
An address association between the paired devices that is stored for the duration of the pairing
A regular beacon message that transmits one arbitrary 8-bit sensor value every 10 seconds while the devices are paired
A method for confirming to the sender that the message was successfully received by the receiver


## Synopsis

I implemented a protocol that allows for simulation of how two fictional devices would pair, in the model of how a headset might be paired with a phone. In order to accomplish this, I did research on how bluetooth technology works. Bluetooth devices communicate over radio waves using Adaptive Frequency-Hopping, which is a method of dealing with potential interference caused by other household appliances using overlapping frequency bands; the bluetooth devices bypass this potential issue by rapidly hopping between radio wave frequencies, this band-hopping is also a security measure in case someone is sniffing a given frequency bang. I created two 'devices' in firstDevice.c and secondDevice.c to simulate this effect. The devices send a hello message for initialization, send an 8 bit sensor value then confirm to each other proper reception of this signal using a simple checksum.

## Method

I use regular C sockets to simulate radio wave frequencies, the first device is a 'server' and the second device is the 'client'. This is how I simulate the bluetooth discovery stage. After the devices connect, they send hello messages back and forth. On alternating turns each device sends a packet with an arbitrary 8-bit sensor value, this happens at 10 second intervals. A checksum is also included in the packet, the checksum is very simple (just adding 5 to the character representing the sensor value), this is effective because the sensor value is just a single byte and easy to verify. The receiving device sends a confirmation message to the sender, or a '0' if the packet was lost, the sending device outputs if the packet was successfully sent or not.

The address association was the biggest challenge of this mini-project, the two devices need to know what the next 'frequency' or port is going to be, without communicating it to the other device (that would defeat the point of hopping frequencies). I used the principle deterministic random number generation to solve this issue, because computers can only generate a random number deterministically, if the two devices start from the same seed frequency, they can jump to 'random' frequencies only guessable by someone who knows the seed frequency and the algorithm used to generate the random number. I used the srand() C method, but in an actual implementation a less commonly used random number generator would probably be prudent.

In this way I have the devices connecting and communicating over a new port every 10 seconds, which simulates the frequency hopping bluetooth devices would do. In this way the devices have an address association for the duration of the connection.

## Running it

To run the devices, navigate to the project repository and type
  $ Make
I ran the devices on the same computer with two terminals, but feel free to run it on two separate computers, just change the value of IP_VAL on the secondDevice.c file to the name of the computer the firstDevice is run on
run ./firstDevice on the first terminal/computer
run ./secondDevice on the second terminal/computers

Should work like a charm, I ran it on a Macbook Pro

I've included a link to a video showing how it works on my computer
https://youtu.be/iQbd_yxTYGg
