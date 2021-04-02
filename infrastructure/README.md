# Example AWS DynamoDB state tracking

This solution provides a very simple way to create a cognito authenticated user that can add entries to a persistent database. In this example new users are automatically registered through a auto confirmation function for simplicity sake. In production this would typically be done by email validation.

This example creates the following AWS resources:
* DynamoDB table - for score keeping
* Cognito user pool
* API GW/Lambda for proxying auth and DB requests


## Deploy

Serverless Framework managed https://www.serverless.com

Configure your AWS credentials either by:
* ~/.aws/credentials profile
* environment variables
* Role assigned to an EC2 instance

### Install serverless plugin requirements (first time only)
```
cd infrastructure
sls plugin install --name serverless-stack-output
```

#### Serverless invocation
```
cd infrastructure
sls deploy
```

## Testing
Use the provided `unit-test.sh` to test out the functionality. It'll perform the following actions:
* create a new user that is authenticated
* save a score value
* retrieve the score value
