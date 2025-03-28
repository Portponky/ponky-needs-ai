extends StaticBody2D


func plan(person: Person, action: Action) -> void:
	if action == %GetWorker:
		person.workstation = self
		action.active = false
		
		var tags = %DoWork.tags.duplicate()
		tags.append(person.full_name)
		%DoWork.set_tags(tags)
		
		%DoWork.active = true
		person.do_walk_to(%WorkPos.global_position)
		person.do_wait(8.0, 12.0)
		person.do_reward(action.advert)
	elif action == %DoWork:
		if !person.get_node("Agent").tags.has(action.tags.front()):
			print("Person %s trying to do incorrect work! " % [person.full_name], action.tags)
		person.do_walk_to(%WorkPos.global_position)
		person.do_wait(3.0, 5.0)
		# no reward lol
