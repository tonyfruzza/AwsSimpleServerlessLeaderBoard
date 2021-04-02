#!/bin/sh
REGION=us-west-2
USER_NAME=tester
PASSSWORD=testing123
API_GW_ENDPOINT=$(aws --region $REGION cloudformation describe-stacks --stack-name GameStateManager-dev --query 'Stacks[0].Outputs[?OutputKey == `ServiceEndpoint`].{out:OutputValue}' --output text)
AUTH_RESP=$(curl -s -XPOST $API_GW_ENDPOINT/create_user -d "{\"username\": \"$USER_NAME\", \"password\": \"$PASSSWORD\"}" |jq -r ".msg")
curl -H "Authorization: $AUTH_RESP" -XPOST $API_GW_ENDPOINT/put_highscore -d "{\"game_id\": \"game_session_123\", \"username\": \"$USER_NAME\", \"score\": \"2400\"}"
curl -H "Authorization: $AUTH_RESP" $API_GW_ENDPOINT/get_highscore
