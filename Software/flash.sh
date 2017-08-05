#!/bin/sh

# Simple flash file for programming blank badges

nrfjprog -f nrf52 --recover
nrfjprog -f nrf52 --program DC25PartyBadge_merged_user.hex --chiperase
nrfjprog -f nrf52 --reset
