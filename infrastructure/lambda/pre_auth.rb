def lambda_handler(event:, context:)
  event['response'] = {autoConfirmUser: true}
  return event
end
