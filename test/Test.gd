extends Node2D

func _ready() -> void:
	await get_tree().process_frame
	for n in 10:
		await get_tree().create_timer(0.5).timeout
		$Obj.position += 1*Vector2.ONE
	$Person.think()
