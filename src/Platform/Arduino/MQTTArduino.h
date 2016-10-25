/*******************************************************************************
 * Copyright (c) 2014 IBM Corp.
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Allan Stockdill-Mander - initial API and implementation and/or initial documentation
 *******************************************************************************/

#if !defined(__MQTT_ARDUINO_)
#define __MQTT_ARDUINO_


#if defined(__cplusplus)
extern "C" {
#endif

	typedef struct Timer
	{
		unsigned long interval_end_ms;
	} Timer;

	void TimerInit(Timer*);
	char TimerIsExpired(Timer*);
	void TimerCountdownMS(Timer*, unsigned int);
	void TimerCountdown(Timer*, unsigned int);
	int TimerLeftMS(Timer*);

	typedef struct Network
	{
		void* client;
		int chunkSize;
		int(*mqttread) (struct Network*, unsigned char*, int, int);
		int(*mqttwrite) (struct Network*, unsigned char*, int, int);
	} Network;

	int arduino_read(Network*, unsigned char*, int, int);
	int arduino_write(Network*, unsigned char*, int, int);

	void NetworkInit(Network*, void*, int);
	int NetworkConnect(Network*, char*, int);
	void NetworkDisconnect(Network*);
	int NetworkConnected(Network*);

#if defined(__cplusplus)
}
#endif

#endif
