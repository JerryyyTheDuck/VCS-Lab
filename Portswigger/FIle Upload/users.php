<?php

include "../config.php";
include_once "app/functions.php";
include_once "app/managers/AccountManager.php";

session_start();

$link = ConnectDB();

$accountManager = new AccountManager($link);
$accountManager->CheckAuth();

$page_name = "Users";

$is_admin = ($_SESSION['user_group'] === 'Administrator') ? 1 : 0;

if(!$is_admin)
{
    header("Location: dashboard");
    exit();
}

include_once 'app/pages/header.php';

?>         
<div class="body-wrapper">
    <div class="container-fluid">
        
        <div class="card card-body py-3">
            <div class="row align-items-center">
                <div class="col-12">
                    <div class="d-sm-flex align-items-center justify-space-between">
                        <h4 class="mb-4 mb-sm-0 card-title">Users List</h4>
                        <nav aria-label="breadcrumb" class="ms-auto">
                            <a href="javascript:void(0)" id="btn-add-contact" class="btn btn-xs btn-primary align-items-center" onclick="createUserModal();">Create User</a>
                        </nav>
                    </div>
                </div>
            </div>
        </div>
        
        <style>
        #users_table td {white-space: normal;word-wrap: break-word;}
        </style>

        <table id="users_table" class="table text-nowrap customize-table mb-0 align-middle" >
            <thead class="text-dark fs-4">
                <tr>
                    <th>
                        <h6 class="fs-4 fw-semibold mb-0">ID</h6>
                    </th>
                    <th>
                        <h6 class="fs-4 fw-semibold mb-0">Login</h6>
                    </th>
                    <th>
                        <h6 class="fs-4 fw-semibold mb-0">2FA</h6>
                    </th>
                    <th>
                        <h6 class="fs-4 fw-semibold mb-0">Builds</h6>
                    </th>
                    <th>
                        <h6 class="fs-4 fw-semibold mb-0">Created</h6>
                    </th>
                    <th>
                        <h6 class="fs-4 fw-semibold mb-0">Last Login</h6>
                    </th>
                    <th>
                        <h6 class="fs-4 fw-semibold mb-0">Status</h6>
                    </th>
                    <th>
                        <h6 class="fs-4 fw-semibold mb-0">Actions</h6>
                    </th>
                </tr>
            </thead>
            <tbody>

            </tbody>
        </table>
    </div>
</div>
</div>

<!-- Modal Create User -->
<div class="modal fade" id="create_user_modal" data-bs-backdrop="static" data-bs-keyboard="false" tabindex="-2">
    <div class="modal-dialog modal-dialog-scrollable modal-lg">
        <div class="modal-content">
            <div class="modal-header d-flex align-items-center">
                <h4 class="modal-title" id="modal_create_rule_header">Create User</h4>
                <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
            </div>
            
            <div class="modal-body">
                <div class="card">
                    <div>
                    
                        <div class="form-body">
                            <div class="row">
                                <div class="col-md-12">
                                    <div class="mb-3">
                                        <input type="text" class="form-control" placeholder="Username" id="create_user_username">
                                    </div>
                                </div>
                            </div>

                            <div class="row">
                                <div class="col-md-12">
                                    <div class="mb-3">
                                        <input type="password" class="form-control" placeholder="Password" id="create_user_password">
                                    </div>
                                </div>
                            </div>

                            <div class="row">
                                <div class="col-md-12">
                                    <div class="mb-3">
                                        <input type="password" class="form-control" placeholder="Confirm password" id="create_user_confirm_password">
                                    </div>
                                </div>
                            </div>

                            <div class="row">
                                <div class="col-md-9">
                                    <div class="mb-3">
                                    <select id="create_user_builds" class="form-control" ></select>
                                    </div>
                                </div>

                                <div class="col-md-3">
                                    <div class="mb-3">
                                        <div class="form-group mb-4">
                                            <select class="form-select" id="create_user_role" >
                                                <option value="Worker" selected>Worker</option>
                                                <option value="Administrator">Administrator</option>
                                            </select>
                                        </div>
                                    </div>
                                </div>
                            </div>

                        </div>
                    </div>
                </div>
                <div class="modal-footer">
                    <a id="modal_create_button" href="#" class="btn btn-primary" >Create</a>
                    <button type="button" class="btn bg-danger-subtle text-danger  waves-effect text-start" data-bs-dismiss="modal">Close</button>
                </div>
            </div>
        </div>
    </div>        
</div>

<style>
#sessionsTable td {white-space: normal;word-wrap: break-word;}
</style>

<!-- Modal Session List -->
<div class="modal fade" id="modal_session_list" data-bs-backdrop="static" data-bs-keyboard="false" tabindex="-2">
    <div class="modal-dialog modal-xl">
        <div class="modal-content">
            <div class="modal-header d-flex align-items-center">
                <h4 class="modal-title" id="session_list_modal_header">Sessions List</h4>
                <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
            </div>
                    
            <div class="modal-body-downloads">
                <table id="sessionsTable" class="table text-nowrap mb-0 align-middle">
                    <thead>
                        <tr>
                            <th><h6 class="fs-4 fw-semibold mb-0">ID</h6></th>
                            <th><h6 class="fs-4 fw-semibold mb-0">Session ID</h6></th>
                            <th><h6 class="fs-4 fw-semibold mb-0">User Agent</h6></th>
                            <th><h6 class="fs-4 fw-semibold mb-0">Status</h6></th>
                            <th><h6 class="fs-4 fw-semibold mb-0">Created</h6></th>
                            <th><h6 class="fs-4 fw-semibold mb-0">Last Activity</h6></th>
                            <th></th>
                        </tr>
                    </thead>
                    <tbody>
                    </tbody>
                </table>
            </div>
            <div class="modal-footer">
                <button type="button" class="btn bg-danger-subtle text-danger waves-effect text-start" data-bs-dismiss="modal">Close</button>
            </div>
        </div>
    </div>
</div>

<!-- Modal Edit User -->
<div class="modal fade" id="edit_user_modal" data-bs-backdrop="static" data-bs-keyboard="false" tabindex="-2">
    <div class="modal-dialog modal-dialog-scrollable modal-lg">
        <div class="modal-content">
            <div class="modal-header d-flex align-items-center">
                <h4 class="modal-title" id="modal_edit_user_header">Edit User #?</h4>
                <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
            </div>
            
            <div class="modal-body">
                <div class="card">
                    <div>
                    
                        <div class="form-body">
                        <input type="text" class="form-control" placeholder="id" id="edit_user_id" style="display: none;">
                            <div class="row">
                                <div class="col-md-12">
                                    <div class="mb-3">
                                        <input type="text" class="form-control" placeholder="Username" id="edit_user_username">
                                    </div>
                                </div>
                            </div>

                            <div class="row">
                                <div class="col-md-12">
                                    <div class="mb-3">
                                        <input type="password" class="form-control" placeholder="Password" id="edit_user_password">
                                    </div>
                                </div>
                            </div>

                            <div class="row">
                                <div class="col-md-12">
                                    <div class="mb-3">
                                        <input type="password" class="form-control" placeholder="Confirm password" id="edit_user_confirm_password">
                                    </div>
                                </div>
                            </div>

                            <div class="row">
                                <div class="col-md-9">
                                    <div class="mb-3">
                                    <select id="edit_user_builds" class="form-control" ></select>
                                    </div>
                                </div>

                                <div class="col-md-3">
                                    <div class="mb-3">
                                        <div class="form-group mb-4">
                                            <select class="form-select" id="edit_user_role" >
                                                <option value="Worker" selected>Worker</option>
                                                <option value="Administrator">Administrator</option>
                                            </select>
                                        </div>
                                    </div>
                                </div>
                            </div>

                            <div class="row">
                                <div class="col-md-9">
                                    <div class="form-check form-check-inline">
                                        <input class="form-check-input" type="checkbox" id="edit_user_twofa_disable" value="option1">
                                        <label class="form-check-label" for="edit_user_twofa_disable">Disable 2FA</label>
                                    </div>
                                </div>
                            </div>

                        </div>
                    </div>
                </div>
                <div class="modal-footer">
                    <a id="modal_edit_button" href="#" class="btn btn-primary" >Save</a>
                    <button type="button" class="btn bg-danger-subtle text-danger  waves-effect text-start" data-bs-dismiss="modal">Close</button>
                </div>
            </div>
        </div>
    </div>        
</div>


<div class="dark-transparent sidebartoggler"></div>

<script>var server_time = "<?php echo (new DateTime())->format('Y-m-d H:i:s'); ?>";</script>

<?php include_once 'app/pages/footer.php'; ?>

<script src="assets/js/users.js"></script>
</body>
</html>