#!/bin/bash

protoc --cpp_out=. fmsmoov.proto
cp fmsmoov* /opt/repos/smoovcontrol/proto/
