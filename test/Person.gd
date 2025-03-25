extends CharacterBody2D



func think() -> void:
	$Agent.choose_action(200.0, 300.0)


func _on_agent_action_chosen(action: Action) -> void:
	print(action)


func _on_agent_no_action_chosen() -> void:
	print("No action")
