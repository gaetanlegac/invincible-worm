#include <winternl.h>
#pragma comment(lib,"ntdll.lib")
EXTERN_C NTSTATUS NTAPI RtlAdjustPrivilege(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
EXTERN_C NTSTATUS NTAPI NtSetInformationProcess(HANDLE, ULONG, PVOID, ULONG);

BOOL Protection = true;

void ModeCritique(BOOL Activer) {
	NTSTATUS status;

	// Vérification des droits
	BOOLEAN bl;
	if (NT_SUCCESS(RtlAdjustPrivilege(20, TRUE, FALSE, &bl))) {
		ULONG BreakOnTermination;
		if (Activer) BreakOnTermination = 1;
		else BreakOnTermination = 0;
		// Application
		status = NtSetInformationProcess((HANDLE)-1, 0x1d, &BreakOnTermination, sizeof(ULONG));
		// Verif si reussite

		if (status == 0) {
			Protection = Activer;
#ifdef Mode_Dev
			string EtatChaine = Activer ? "Active" : "Desactive";
			Debug("Protection du processus: " + EtatChaine);
#endif
		}
#ifdef Mode_Dev
		else
			Debug("Impossible de modifier l'etat du mode critique à " + Activer);
#endif
	}
}
