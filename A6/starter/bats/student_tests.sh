#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

setup() {
    ./dsh -s &
    SERVER_PID=$!
    sleep 1
}

teardown() {
    kill $SERVER_PID
}

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Server pipes commands and returns correct output to client" {
    run ./dsh -c <<EOF 
echo hello | wc -l
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:][:cntrl:]')
    expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>1dsh4>cmdloopreturned0"

    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$expected_output" = "$stripped_output" ]
    [ "$status" -eq 0 ]
}

@test "Server handles multiple piped commands" {
    run ./dsh -c <<EOF 
ls | grep dsh | wc -l
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:][:cntrl:]')
    expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>4dsh4>cmdloopreturned0" 

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$expected_output" = "$stripped_output" ]
    [ "$status" -eq 0 ]
}

@test "Server executes command with arguments and returns correct output" {
    run ./dsh -c <<EOF 
cat bats/student_tests.sh | wc -l
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:][:cntrl:]')
    expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>88dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$expected_output" = "$stripped_output" ]
    [ "$status" -eq 0 ]
}

@test "Client stops properly when using exit command" {
    run ./dsh -c <<EOF 
exit
EOF
    stripped_output=$(echo "$output" | tr -d '[:space:][:cntrl:]')
    expected_output="socketclientmode:addr:127.0.0.1:1234dsh4>cmdloopreturned0"

    echo "Captured stdout:"
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    [ "$expected_output" = "$stripped_output" ]
    [ "$status" -eq 0 ]
}
