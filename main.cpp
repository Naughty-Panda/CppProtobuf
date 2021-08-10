//////////////////////////////////////////
//	C++ Protocol Buffers
//	Naughty Panda @ 2021
//////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <execution>

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

private:
	Protobuf::StudentGroup _group;

public:
	void Open() override;
	void Save() override;

	double GetAverageScore(const Protobuf::FullName& name) override;
	std::string GetAllInfo(const Protobuf::FullName& name) override;
	std::string GetAllInfo() override;

	void AddStudent(Protobuf::FullName& name, const std::vector<google::protobuf::uint32>& grades);
	void Clear() { _group.clear_students(); _group.Clear(); }
};

void StudentsGroup::Open() {

	std::ifstream in("StudentsGroup.bin", std::ios_base::binary);

	if (!_group.ParseFromIstream(&in)) {

		std::cerr << "Error parsing file!\n";
	}
}

void StudentsGroup::Save() {

	std::ofstream out("StudentsGroup.bin", std::ios_base::binary);

	if (!_group.SerializeToOstream(&out)) {

		std::cerr << "Error writing file!\n";
	}
}

double StudentsGroup::GetAverageScore(const Protobuf::FullName& name) {

	double result = 0.0;

	std::for_each(std::execution::par, _group.students().begin(), _group.students().end(), [&](const Protobuf::Student& i) {
		
		if (i.full_name().SerializeAsString() == name.SerializeAsString()) {

			result = static_cast<double>(i.avg_score());
		}
	});

	return result;
}

std::string StudentsGroup::GetAllInfo(const Protobuf::FullName& name) {

	std::stringstream result;

	auto print_result = [&](const Protobuf::Student& i) {

		if (i.full_name().SerializeAsString() == name.SerializeAsString()) {

			result << "\nStudent: ";
			result << i.full_name().first_name() << ' ';
			result << i.full_name().last_name() << ' ';
			if (i.full_name().has_sur_name()) {

				result << i.full_name().sur_name();
			}

			result << "\nAverage score: " << i.avg_score();
			result << "\nGrades: ";
			std::copy(i.grades().begin(), i.grades().end(), std::ostream_iterator<int>(result, " "));
			result << '\n';
		}
	};

	std::for_each(std::execution::par, _group.students().begin(), _group.students().end(), print_result);
	return std::move(result.str());
}

std::string StudentsGroup::GetAllInfo() {

	std::stringstream result;
	result << "All students:\n";

	std::for_each(std::execution::seq, _group.students().begin(), _group.students().end(), [&](const Protobuf::Student& i) {

		result << GetAllInfo(i.full_name());
	});

	return std::move(result.str());
}

void StudentsGroup::AddStudent(Protobuf::FullName& name, const std::vector<google::protobuf::uint32>& grades) {

	auto student = _group.add_students();
	student->set_allocated_full_name(&name);
	google::protobuf::uint32 grades_sum = 0;

	std::for_each(std::execution::par, grades.begin(), grades.end(), [&](const auto& i) { student->add_grades(i); grades_sum += i; });

	student->set_avg_score(grades_sum / static_cast<float>(grades.size()));
}



int main() {
	
	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	// Seting up Protobuf::FullNames for our group
	Protobuf::FullName name1, name2, name3, name4, name5, name6;
	name1.set_first_name("Mark");
	name1.set_last_name("Tolstoy");
	name1.set_sur_name("Fedorovich");

	name2.set_first_name("Tamara");
	name2.set_last_name("Lomina");

	name3.set_first_name("Ivan");
	name3.set_last_name("Ushakov");

	name4.set_first_name("Maria");
	name4.set_last_name("Tarasova");
	name4.set_sur_name("Igorevna");

	name5 = name2;
	name6 = name1;

	// Creating and filling StudentsGroup
	StudentsGroup OldGroup;
	OldGroup.AddStudent(name1, { 4,5,4,4,3,5 });
	OldGroup.AddStudent(name2, { 3,3,5,4,3 });
	OldGroup.AddStudent(name3, { 4,3,5,3,4 });
	OldGroup.AddStudent(name4, { 5,5,5,4,5,4 });

	// Serialize data on disk and cleanup
	OldGroup.Save();
	OldGroup.Clear();

	// Deserialize data from disk to a new group
	StudentsGroup NewGroup;
	NewGroup.Open();

	// Testing StudentsGroup methods with NewGroup
	std::cout << "GetAllInfo():\n\n" << NewGroup.GetAllInfo();
	std::cout << "\nGetAllInfo(name):\n" << NewGroup.GetAllInfo(name5);
	std::cout << "\nGetAverageScore(name): " << NewGroup.GetAverageScore(name6);

	// Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();

	return 0;
}
