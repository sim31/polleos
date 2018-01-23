#!/bin/bash

# Normal polls
declare -a polls=(
  '{"question": "What do you think about this app?", "options": ["Very good", "Not bad", "Bad"]}'
  '{"question": "Some question?", "options": ["A", "B", "C"]}'
  '{"question": "When will eos moon?", "options": ["Tomorrow", "Next week", "Next year", "Its already on the moon", "What is moon?"]}'
  '{"question": "One option", "options": ["option1"]}'
  )

# Token polls
declare -a token_polls=(
  '{"question": "What will be the price of eos by the end of 2018", "options": ["14$", "20$", "50$", "100$", "200$"], "token": "eos"}'
  '{"question": "What will be the price of currency token by the end of 2018", "options": ["14$", "20$", "50$", "100$", "200$"], "token": "currency"}'
    )

for p in "${polls[@]}"; do
  # echo "${p}"
  eosc push message polleos newoptpoll "${p}" -S polleos
done

for p in "${token_polls[@]}"; do
  # echo "${p}"
  eosc push message polleos newtokenpoll "${p}" -S polleos
done

vote_cmd="eosc push message polleos vote"

$vote_cmd '{"id": 1, "option": 1, "voter": "inita"}' -S polleos,inita -p inita@active
$vote_cmd '{"id": 0, "option": 2, "voter": "inita"}' -S polleos,inita -p inita@active
$vote_cmd '{"id": 1, "option": 2, "voter": "initb"}' -S polleos,initb -p initb@active
$vote_cmd '{"id": 0, "option": 3, "voter": "initb"}' -S polleos,initb -p initb@active
$vote_cmd '{"id": 3, "option": 1, "voter": "initb"}' -S polleos,initb -p initb@active
$vote_cmd '{"id": 5, "option": 1, "voter": "initb"}' -S polleos,initb -p initb@active

echo ""
echo "Poll table:"
eosc get table polleos polleos optpoll

  








