#pragma once

class SerializedClassExample {
public:
	SerializedClassExample() = default;
	~SerializedClassExample() = default;

	void ImGui() {
		ImGui::InputText("A string", &myString);
	}

private:
	std::string myString = "This field is serialized ! If you modify it, close the app and re-open it, your changes will remain !";

private:
	//Serialization
	friend class cereal::access;
	template<class Archive>
	void serialize(Archive& archive)
	{
		archive(
			cereal::make_nvp("A random string", myString)
		);
	}
	// Check the documentation of Cereal for full information on all the things you can do !
	// https://uscilab.github.io/cereal/
	// PS : Cereal is amazing !
};