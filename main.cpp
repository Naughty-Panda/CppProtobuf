//////////////////////////////////////////
//	C++ Protocol Buffers
//	Naughty Panda @ 2021
//////////////////////////////////////////

#include <iostream>
#include <fstream>

// Protobuf RELEASE libs
#pragma comment (lib, "lib/libprotobuf.lib")
#pragma comment (lib, "lib/libprotobuf-lite.lib")
#pragma comment (lib, "lib/libprotoc.lib")

// Protobuf DEBUG libs
#pragma comment (lib, "lib/libprotobufd.lib")
#pragma comment (lib, "lib/libprotobuf-lited.lib")
#pragma comment (lib, "lib/libprotocd.lib")

//////////////////////////////////////////
// 7.2
//////////////////////////////////////////

#include "Classes.pb.h"

//////////////////////////////////////////
// 7.3
//////////////////////////////////////////

class IRepository {

	virtual void Open() = 0;
	virtual void Save() = 0;
};


class IMethods {

	virtual double GetAverageScore(const Protobuf::FullName& name) = 0;
	virtual std::string GetAllInfo(const Protobuf::FullName& name) = 0;
	virtual std::string GetAllInfo() = 0;
};

class StudentsGroup : public IRepository, public IMethods {

public:
	Protobuf::StudentGroup _group;

	void Open() override;
	void Save() override;

	double GetAverageScore(const Protobuf::FullName& name) override;
	std::string GetAllInfo(const Protobuf::FullName& name) override;
	std::string GetAllInfo() override;

	void AddStudent(Protobuf::FullName& name);
};

void StudentsGroup::Open() {

	std::ifstream in("StudentsGroup.bin", std::ios_base::binary);

	if (!_group.ParseFromIstream(&in)) {

		std::cerr << "Cannot parse file!\n";
	}
}

void StudentsGroup::Save() {

	std::ofstream out("StudentsGroup.bin", std::ios_base::binary);
	_group.SerializeToOstream(&out);
}

double StudentsGroup::GetAverageScore(const Protobuf::FullName& name) {

	for (const auto& student : _group.students()) {

		if (student.full_name().SerializeAsString() == name.SerializeAsString()) {

			return static_cast<double>(student.avg_score());
		}
	}

	return 0.0;
}

std::string StudentsGroup::GetAllInfo(const Protobuf::FullName& name) {

	return " ";
}

std::string StudentsGroup::GetAllInfo() {

	return " ";
}

void StudentsGroup::AddStudent(Protobuf::FullName& name) {

	auto student = _group.add_students();
	student->set_allocated_full_name(&name);
}



int main() {
	
	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;


	Protobuf::FullName name;
	name.set_first_name("Hello");

	std::cout << name.first_name();

	/*Protobuf::FullName fn;
	fn.set_first_name("Vasiliy");
	fn.set_last_name("Petrov");
	Protobuf::Student std;
	std.set_allocated_full_name(&fn);

	StudentsGroup group;
	auto st = group._group.add_students();
	st->set_allocated_full_name(&fn);
	st->set_avg_score(4.6f);
	st->add_grades(4);
	st->add_grades(5);
	st->add_grades(3);
	st->add_grades(4);
	group.Save();*/

	StudentsGroup group;
	group.Open();

	std::cout << group._group.students(0).full_name().first_name() << '\n';
	std::cout << group._group.students(0).full_name().last_name() << '\n';
	std::cout << group._group.students(0).full_name().has_sur_name() << '\n';
	std::cout << group._group.students(0).avg_score() << '\n';
	std::cout << group._group.students(0).grades(2) << '\n';



	// Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
